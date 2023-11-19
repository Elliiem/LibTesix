#include "Overlay.h"

#include "rapidjson/pointer.h"

#include <fstream>
#include <sstream>

namespace LibTesix {
//

Overlay::Overlay() {
}

Overlay::Overlay(uint32_t width, uint32_t height) {
    this->width = width;
    this->height = height;

    lines.resize(height);
}

void Overlay::Clear() {
    for(StyledSegmentArray& arr : lines) {
        arr.Clear();
    }
}

void Overlay::Box(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Style style, const char* right, const char* left, const char* top,
    const char* bottom, const char* top_right, const char* top_left, const char* bottom_right, const char* bottom_left) {
    icu::UnicodeString top_str;
    top_str.append(top_right);
    for(uint32_t i = 0; i < width - 2; i++) {
        top_str.append(icu::UnicodeString(top));
    }
    top_str.append(top_left);

    StyledSegmentArray middle;
    middle.Add(right, style, 0);
    middle.Add(left, style, width - 1);

    icu::UnicodeString bottom_str;
    bottom_str.append(bottom_right);
    for(uint32_t i = 0; i < width - 2; i++) {
        bottom_str.append(icu::UnicodeString(bottom));
    }
    bottom_str.append(bottom_left);

    lines[0].Add(top_str, style, 0);

    for(uint32_t i = 1; i < height - 1; i++) {
        lines[i] = middle;
    }

    lines[height - 1].Add(bottom_str, style, 0);
}

void Overlay::Box(Style style, const char* right, const char* left, const char* top, const char* bottom, const char* top_right, const char* top_left,
    const char* bottom_right, const char* bottom_left) {
    UpdateWidth();
    Box(0, 0, width, height, style, right, left, top, bottom, top_right, top_left, bottom_right, bottom_left);
}

void Overlay::UpdateWidth() {
    uint32_t new_width {};

    for(StyledSegmentArray& arr : lines) {
        for(StyledSegment& seg : arr.segments) {
            if(seg.start + seg.str.length() > new_width) {
                new_width = seg.start + seg.str.length();
            }
        }
    }
    width = new_width;
}

Style GetSegmentStyle(rapidjson::Value& segment) {
    Style style;

    style.FG(Color(segment["style"]["fg"]["r"].GetInt(), segment["style"]["fg"]["g"].GetInt(), segment["style"]["fg"]["b"].GetInt()));
    style.BG(Color(segment["style"]["bg"]["r"].GetInt(), segment["style"]["bg"]["g"].GetInt(), segment["style"]["bg"]["b"].GetInt()));
    style.Bold(segment["style"]["modifiers"]["thickness"].GetInt() == 1 ? true : false);
    style.Faint(segment["style"]["modifiers"]["thickness"].GetInt() == 2 ? true : false);
    style.Blinking(segment["style"]["modifiers"]["blinking"].GetInt());
    style.Reverse(segment["style"]["modifiers"]["reverse"].GetInt());
    style.Underlined(segment["style"]["modifiers"]["underlined"].GetInt());
    style.Italic(segment["style"]["modifiers"]["italic"].GetInt());

    return style;
}

Overlay ReadOverlay(rapidjson::Document& json, const char* name) {
    if(!json.HasMember(name)) return Overlay(1, 1);
    rapidjson::Value& lines = json[name]["lines"];

    Overlay overlay;
    overlay.height = lines.Size();
    overlay.lines.resize(overlay.height);

    uint32_t width = 0;

    for(uint32_t i = 0; i < overlay.height; i++) {
        rapidjson::Value& line = lines[i];
        for(uint32_t j = 0; j < line["segments"].Size(); j++) {
            rapidjson::Value& segment = line["segments"][j];
            overlay.lines[i].Add(segment["string"].GetString(), GetSegmentStyle(segment), segment["start"].GetInt());
        }

        uint32_t len = line["segments"][line["segments"].Size() - 1]["start"].GetInt() +
                       line["segments"][line["segments"].Size() - 1]["string"].GetStringLength();
        if(len > width) {
            width = len;
        }
    }

    overlay.width = width;

    return overlay;
}

rapidjson::Document OpenJson(const char* filename) {
    // open file
    std::ifstream file;
    file.open(filename);
    // read file
    std::stringstream stringstream;
    stringstream << file.rdbuf();
    std::string json = stringstream.str();

    // make and return doc
    rapidjson::Document ret;
    ret.Parse(json.c_str());

    file.close();
    return ret;
}

void SaveJson(rapidjson::Document& json, const char* filename) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);

    std::ofstream file;
    file.open(filename, std::ios::out);
    file.write(buffer.GetString(), buffer.GetSize());
    file.close();
}

rapidjson::Value CreateCol(Color col, rapidjson::Document& json) {
    rapidjson::Value color(rapidjson::kObjectType);
    rapidjson::Value r(col.r);
    rapidjson::Value g(col.g);
    rapidjson::Value b(col.b);

    color.AddMember("r", r, json.GetAllocator());
    color.AddMember("g", g, json.GetAllocator());
    color.AddMember("b", b, json.GetAllocator());

    return color;
};

rapidjson::Value CreateSegment(StyledSegment& seg, rapidjson::Document& json) {
    // segment
    rapidjson::Value segment(rapidjson::kObjectType);

    // start
    segment.AddMember("start", seg.start, json.GetAllocator());

    // string
    rapidjson::Value string(rapidjson::kStringType);
    std::string string_val;
    seg.str.toUTF8String(string_val);
    string.SetString(string_val.c_str(), string_val.length(), json.GetAllocator());

    segment.AddMember("string", string, json.GetAllocator());

    // style
    rapidjson::Value style(rapidjson::kObjectType);

    // fg
    rapidjson::Value fg = CreateCol(seg.style.col.fg, json);
    style.AddMember("fg", fg, json.GetAllocator());

    // bg
    rapidjson::Value bg = CreateCol(seg.style.col.bg, json);
    style.AddMember("bg", bg, json.GetAllocator());

    // modifiers
    rapidjson::Value modifiers(rapidjson::kObjectType);

    rapidjson::Value thickness(rapidjson::kNumberType);
    thickness.SetInt(seg.style.bool_state[Style::BOLD] ? 1 : 0);
    thickness.SetInt(seg.style.bool_state[Style::BOLD] && thickness.GetInt() == 0 ? 2 : 0);
    modifiers.AddMember("thickness", thickness, json.GetAllocator());

    modifiers.AddMember("blinking", seg.style.bool_state[Style::BLINKING], json.GetAllocator());

    modifiers.AddMember("reverse", seg.style.bool_state[Style::REVERSE], json.GetAllocator());

    modifiers.AddMember("underlined", seg.style.bool_state[Style::UNDERLINED], json.GetAllocator());

    modifiers.AddMember("italic", seg.style.bool_state[Style::ITALIC], json.GetAllocator());

    style.AddMember("modifiers", modifiers, json.GetAllocator());

    segment.AddMember("style", style, json.GetAllocator());

    return segment;
}

void AddToDocument(rapidjson::Value& val, std::string& name, rapidjson::Document& json) {
    rapidjson::Value key(rapidjson::kStringType);
    key.SetString(name.c_str(), name.length(), json.GetAllocator());

    if(!json.HasMember(name.c_str())) {
        json.AddMember(key, val, json.GetAllocator());
    } else {
        json.RemoveMember(name.c_str());

        rapidjson::Value overlay(rapidjson::kObjectType);
        overlay.AddMember("lines", val, json.GetAllocator());

        json.AddMember(key, overlay, json.GetAllocator());
    }
}

void WriteOverlay(Overlay& overlay, std::string& name, rapidjson::Document& json) {
    rapidjson::Value lines(rapidjson::kArrayType);

    for(StyledSegmentArray& arr : overlay.lines) {
        rapidjson::Value line(rapidjson::kObjectType);

        rapidjson::Value segments(rapidjson::kArrayType);

        for(StyledSegment& seg : arr.segments) {
            segments.PushBack(CreateSegment(seg, json), json.GetAllocator());
        }

        line.AddMember("segments", segments, json.GetAllocator());
        lines.PushBack(line, json.GetAllocator());
    }

    // Add to document
    AddToDocument(lines, name, json);
}

} // namespace LibTesix