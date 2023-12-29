#include "Json.h"

#include "Overlay.h"
#include "Window.h"

#include <fstream>
#include <sstream>

namespace LibTesix {

Color ReadColor(rapidjson::Value& json_color) {
    Color color;

    uint64_t r = json_color.HasMember("r") ? json_color["r"].GetUint64() : 0;
    uint64_t g = json_color.HasMember("r") ? json_color["g"].GetUint64() : 0;
    uint64_t b = json_color.HasMember("r") ? json_color["b"].GetUint64() : 0;

    color.r = r;
    color.g = g;
    color.b = b;

    return color;
}

enum class Thickness { FAINT = 0, NORMAL = 1, BOLD = 2 };

Style ReadStyle(rapidjson::Value& json_styles, const std::string& name) {
    rapidjson::Value& json_style = json_styles[name.c_str()];

    Style style(name);

    Color fg = json_style.HasMember("fg") ? ReadColor(json_style["fg"]) : Color();
    Color bg = json_style.HasMember("bg") ? ReadColor(json_style["bg"]) : Color();

    style.FG(fg);
    style.BG(bg);

    if(!json_style.HasMember("modifiers")) return style;

    uint64_t thickness = json_style.HasMember("thickness") ? json_style["thickness"].GetUint64() : 0;
    switch(static_cast<Thickness>(thickness)) {
        case Thickness::FAINT:
            style.Faint(true);
            break;
        case Thickness::BOLD:
            style.Bold(true);
            break;
    }

    style.Blinking(json_style.HasMember("blinking") ? json_style["modifiers"]["blinking"].GetBool() : false);
    style.Reverse(json_style.HasMember("reverse") ? json_style["modifiers"]["reverse"].GetBool() : false);
    style.Underlined(json_style.HasMember("underlined") ? json_style["modifiers"]["underlined"].GetBool() : false);
    style.Italic(json_style.HasMember("italic") ? json_style["modifiers"]["italic"].GetBool() : false);

    return style;
}

void AllocStyles(rapidjson::Value& json_styles) {
    for(rapidjson::Value::ConstMemberIterator iter = json_styles.MemberBegin(); iter != json_styles.MemberEnd(); iter++) {
        Style style = ReadStyle(json_styles, iter->name.GetString());
        style_allocator.Add(style);
    }
}

const Style* GetStylePointer(const std::string& name) {
    const Style* style_p = style_allocator[name];

    if(style_p == nullptr) return STANDARD_STYLE;

    return style_p;
}

StyledSegmentArray ReadSegmentArray(rapidjson::Value& json_arr) {
    StyledSegmentArray arr;

    if(!json_arr.HasMember("segments")) return arr;

    rapidjson::Value& json_segments = json_arr["segments"];

    for(uint64_t i = 0; i < json_segments.Size(); i++) {
        std::string str = json_segments[i].HasMember("string") ? json_segments[i]["string"].GetString() : "";
        const Style* style = GetStylePointer(json_segments[i].HasMember("style") ? json_segments[i]["style"].GetString() : "");
        uint64_t start = json_segments[i].HasMember("start") ? json_segments[i]["start"].GetUint64() : 0;

        arr.Add(str.c_str(), style, start);
    }

    return arr;
}

JsonDocument::JsonDocument(const std::string& filepath) {
    // open file
    std::ifstream file;
    file.open(filepath);

    // read file
    std::stringstream stringstream;
    stringstream << file.rdbuf();
    std::string json = stringstream.str();

    // parse json
    doc.Parse(json.c_str());

    file.close();
    this->filepath = filepath;

    if(doc.HasMember("styles")) {
        rapidjson::Value& styles = doc["styles"];
        AllocStyles(styles);
    }
}

bool Window::WriteToJson(JsonDocument& json, const std::string& name) {
}

bool Window::LoadFromJson(JsonDocument& json, const std::string& name) {
    if(!json.doc.HasMember("objects") ? json.doc["objects"].HasMember(name.c_str()) : false) return false;
    if(!json.doc["objects"][name.c_str()].HasMember("type") ? json.doc["objects"][name.c_str()]["type"].GetString() == "window" : false) return false;

    return LoadFromJson(json, json.doc["objects"][name.c_str()]);
}

bool Window::LoadFromJson(JsonDocument& json, rapidjson::Value& json_window) {
    x = json_window.HasMember("x") ? json_window["x"].GetUint64() : 0;
    y = json_window.HasMember("y") ? json_window["y"].GetUint64() : 0;
    width = json_window.HasMember("width") ? json_window["width"].GetUint64() : 0;
    height = json_window.HasMember("height") ? json_window["height"].GetUint64() : 0;

    if(json_window.HasMember("overlay_enabled") ? json_window["overlay_enabled"].GetBool() : false) {
        overlay.LoadFromJson(json, json_window.HasMember("overlay") ? json_window["overlay"].GetString() : "");
        overlay_enabled = true;
        has_overlay = true;
    }

    if(!json_window.HasMember("lines")) return false;
    for(uint64_t i = 0; i < json_window["lines"].Size(); i++) {
        // TODO Make StyledString Loader
        lines.push_back(ReadSegmentArray(json_window["lines"][i]));
    }
}

bool Overlay::WriteToJson(JsonDocument& json, const std::string& name) {
}

bool Overlay::LoadFromJson(JsonDocument& json, const std::string& name) {
    if(!json.doc.HasMember("objects") ? json.doc["objects"].HasMember(name.c_str()) : false) return false;
    if(!json.doc["objects"][name.c_str()].HasMember("type") ? json.doc["objects"][name.c_str()]["type"].GetString() == "overlay" : false)
        return false;

    return LoadFromJson(json.doc["objects"][name.c_str()]);
}

bool Overlay::LoadFromJson(rapidjson::Value& json_overlay) {
    if(!json_overlay.HasMember("lines")) return false;

    height = json_overlay["lines"].Size();

    uint64_t width_c = 0;

    for(uint64_t i = 0; i < json_overlay["lines"].Size(); i++) {
        lines.push_back(ReadSegmentArray(json_overlay["lines"][i]));
        width_c = width_c < lines.back().Len() ? lines.back().Len() : width_c;
    }

    width = width_c;

    return true;
}

} // namespace LibTesix