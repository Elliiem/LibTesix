#include "Json.h"

#include "Overlay.h"
#include "Window.h"

#include <fstream>
#include <sstream>

namespace LibTesix {

// Adds a object to a key
void AddToDocument(rapidjson::Value& val, const char* key, uint64_t name_c, rapidjson::Document& json) {
    rapidjson::Value json_key(rapidjson::kStringType);
    json_key.SetString(key, name_c, json.GetAllocator());

    if(!json.HasMember(key)) {
        json.AddMember(json_key, val, json.GetAllocator());
    } else {
        json.RemoveMember(key);

        json.AddMember(json_key, val, json.GetAllocator());
    }
}

Style GetStyle(rapidjson::Value& json_style) {
    Style style;

    style.FG(Color(json_style["fg"]["r"].GetInt(), json_style["fg"]["g"].GetInt(), json_style["fg"]["b"].GetInt()));
    style.BG(Color(json_style["bg"]["r"].GetInt(), json_style["bg"]["g"].GetInt(), json_style["bg"]["b"].GetInt()));
    style.Bold(json_style["modifiers"]["thickness"].GetUint() == 1 ? true : false);
    style.Faint(json_style["modifiers"]["thickness"].GetUint() == 2 ? true : false);
    style.Blinking(json_style["modifiers"]["blinking"].GetBool());
    style.Reverse(json_style["modifiers"]["reverse"].GetBool());
    style.Underlined(json_style["modifiers"]["underlined"].GetBool());
    style.Italic(json_style["modifiers"]["italic"].GetBool());

    return style;
}

void AllocStyles(rapidjson::Value& json_obj) {
    // TODO Add type guards
    rapidjson::Value& json_styles = json_obj["styles"];

    for(uint64_t i = 0; i < json_styles.Size(); i++) {
        Style style = GetStyle(json_styles[i]);

        style_allocator.Add(style, json_styles[i]["name"].GetString());
    }
}

// Create json object
rapidjson::Value CreateColObj(const Color& col, rapidjson::Document& json) {
    rapidjson::Value color(rapidjson::kObjectType);
    rapidjson::Value r(col.r);
    rapidjson::Value g(col.g);
    rapidjson::Value b(col.b);

    color.AddMember("r", r, json.GetAllocator());
    color.AddMember("g", g, json.GetAllocator());
    color.AddMember("b", b, json.GetAllocator());

    return color;
};

rapidjson::Value CreateModifierObj(const Style* style, rapidjson::Document& json) {
    rapidjson::Value json_modifiers(rapidjson::kObjectType);
    json_modifiers.AddMember("thickness", style->GetMod(Style::BOLD) + style->GetMod(Style::FAINT) * 2, json.GetAllocator());
    json_modifiers.AddMember("blinking", style->GetMod(Style::BLINKING), json.GetAllocator());
    json_modifiers.AddMember("reverse", style->GetMod(Style::REVERSE), json.GetAllocator());
    json_modifiers.AddMember("underlined", style->GetMod(Style::UNDERLINED), json.GetAllocator());
    json_modifiers.AddMember("italic", style->GetMod(Style::ITALIC), json.GetAllocator());

    return json_modifiers;
}

rapidjson::Value CreateStyleObj(const Style* style, rapidjson::Document& json) {
    rapidjson::Value json_style(rapidjson::kObjectType);

    rapidjson::Value json_fg = CreateColObj(style->col.fg, json);
    json_style.AddMember("fg", json_fg, json.GetAllocator());

    rapidjson::Value json_bg = CreateColObj(style->col.bg, json);
    json_style.AddMember("bg", json_bg, json.GetAllocator());

    rapidjson::Value json_modifiers = CreateModifierObj(style, json);
    json_style.AddMember("modifiers", json_modifiers, json.GetAllocator());

    return json_style;
}

rapidjson::Value CreateSegmentObj(const StyledSegment& segment, rapidjson::Document& json) {
    rapidjson::Value json_segment(rapidjson::kObjectType);

    // Add start to json_segment
    json_segment.AddMember("start", segment.start, json.GetAllocator());

    // Add string
    rapidjson::Value json_string(rapidjson::kStringType);

    std::string segment_utf8;
    segment.str.toUTF8String(segment_utf8);

    json_string.SetString(segment_utf8.c_str(), segment_utf8.length(), json.GetAllocator());
    json_segment.AddMember("string", json_string, json.GetAllocator());

    // Add style
    rapidjson::Value json_style = CreateStyleObj(segment.style, json);
    json_segment.AddMember("style", json_style, json.GetAllocator());

    return json_segment;
}

// Creates the line array from a vector of lines
rapidjson::Value CreateLineArr(const std::vector<StyledSegmentArray>& lines, rapidjson::Document& json) {
    rapidjson::Value json_lines(rapidjson::kArrayType);

    for(const StyledSegmentArray& array : lines) {
        rapidjson::Value json_line(rapidjson::kObjectType);

        rapidjson::Value json_segments(rapidjson::kArrayType);

        for(const StyledSegment& segment : array.segments) {
            json_segments.PushBack(CreateSegmentObj(segment, json), json.GetAllocator());
        }

        json_line.AddMember("segments", json_segments, json.GetAllocator());
        json_lines.PushBack(json_line, json.GetAllocator());
    }

    return json_lines;
}

rapidjson::Value CreateLineArr(const std::vector<StyledString>& lines, rapidjson::Document& json) {
    rapidjson::Value json_lines(rapidjson::kArrayType);

    for(const StyledSegmentArray& array : lines) {
        rapidjson::Value json_line(rapidjson::kObjectType);

        rapidjson::Value json_segments(rapidjson::kArrayType);

        for(const StyledSegment& segment : array.segments) {
            json_segments.PushBack(CreateSegmentObj(segment, json), json.GetAllocator());
        }

        json_line.AddMember("segments", json_segments, json.GetAllocator());

        json_lines.PushBack(json_line, json.GetAllocator());
    }

    return json_lines;
}

// Load a line from json
void GetLine(StyledSegmentArray& dest, rapidjson::Value& line) {
    for(uint64_t i = 0; i < line["segments"].Size(); i++) {
        const char* str = line["segments"][i]["string"].GetString();
        const Style* style = style_allocator[line["segments"][i]["style"].GetString()];

        uint64_t start = line["segments"][i]["start"].GetInt64();

        dest.Add(str, style, start);
    }
}

// Write this window to a json doc
bool Window::WriteToJson(JsonDocument& json, const char* name, uint64_t name_c) {
    // Get data
    rapidjson::Value json_window_lines = CreateLineArr(lines, json.doc);
    rapidjson::Value json_overlay_lines;

    if(has_overlay) {
        json_overlay_lines = CreateLineArr(overlay.lines, json.doc);
    }

    // Add to Object
    rapidjson::Value json_window(rapidjson::kObjectType);

    json_window.AddMember("x", x, json.doc.GetAllocator());
    json_window.AddMember("y", y, json.doc.GetAllocator());
    json_window.AddMember("width", width, json.doc.GetAllocator());
    json_window.AddMember("height", height, json.doc.GetAllocator());
    json_window.AddMember("lines", json_window_lines, json.doc.GetAllocator());

    json_window.AddMember("overlay_enabled", overlay_enabled, json.doc.GetAllocator());

    rapidjson::Value json_overlay(rapidjson::kObjectType);

    if(has_overlay) {
        json_overlay.AddMember("lines", json_overlay_lines, json.doc.GetAllocator());
    }

    json_window.AddMember("overlay", json_overlay, json.doc.GetAllocator());

    // Add to doc
    AddToDocument(json_window, name, name_c, json.doc);

    return true;
}

// Write this Overlay to a json doc
bool Overlay::WriteToJson(JsonDocument& json, const char* name, uint64_t name_c) {
    rapidjson::Value json_lines = CreateLineArr(lines, json.doc);

    rapidjson::Value json_overlay(rapidjson::kObjectType);
    json_overlay.AddMember("lines", json_lines, json.doc.GetAllocator());

    AddToDocument(json_overlay, name, name_c, json.doc);

    return true;
}

// Load a window from json
bool Window::LoadFromJson(JsonDocument& json, const char* name) {
    rapidjson::Value& json_window = json.doc[name];

    return LoadFromJson(json_window);
}

bool Window::LoadFromJson(rapidjson::Value& json_window) {
    rapidjson::Value& json_overlay = json_window["overlay"];

    AllocStyles(json_window);

    has_overlay = json_overlay.MemberCount() != 0;
    overlay_enabled = json_window["overlay_enabled"].GetBool() && has_overlay;

    Move(json_window["x"].GetInt64(), json_window["y"].GetInt64());
    Resize(json_window["width"].GetUint64(), json_window["height"].GetUint64());

    for(uint64_t i = 0; i < height; i++) {
        GetLine(lines[i], json_window["lines"][i]);
    }

    if(has_overlay) {
        overlay.LoadFromJson(json_overlay);
    }

    return true;
}

// Load a overlay from json
bool Overlay::LoadFromJson(JsonDocument& json, const char* name) {
    rapidjson::Value& json_overlay = json.doc[name];

    return LoadFromJson(json_overlay);
}

bool Overlay::LoadFromJson(rapidjson::Value& json_overlay) {
    rapidjson::Value& json_lines = json_overlay["lines"];

    height = json_lines.Size();
    lines.resize(height);

    for(uint64_t i = 0; i < height; i++) {
        GetLine(lines[i], json_lines[i]);
        if(width < lines[i].Len()) width = lines[i].Len();
    }

    return true;
}

// JsonDocument
JsonDocument::JsonDocument(const char* filepath) {
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
}

void JsonDocument::Save() {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::ofstream file;
    file.open(filepath, std::ios::out);
    file.write(buffer.GetString(), buffer.GetSize());
    file.close();
}

} // namespace LibTesix