#pragma once

#include "Json.h"
#include "SegmentArray.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace LibTesix {
struct Overlay {
    Overlay();
    Overlay(uint64_t width, uint64_t height);
    Overlay(JsonDocument& json, const char* name);
    Overlay(rapidjson::Value& json_overlay);

    void Clear();

    void Box(uint64_t x, uint64_t y, uint64_t width, uint64_t height, const Style* style, const char* right = "┃", const char* left = "┃",
        const char* top = "━", const char* bottom = "━", const char* top_right = "┏", const char* top_left = "┓", const char* bottom_right = "┗",
        const char* bottom_left = "┛");

    void Box(const Style* style, const char* right = "┃", const char* left = "┃", const char* top = "━", const char* bottom = "━",
        const char* top_right = "┏", const char* top_left = "┓", const char* bottom_right = "┗", const char* bottom_left = "┛");

    std::vector<StyledSegmentArray> lines;

    uint64_t height;
    uint64_t width;

    void UpdateWidth();

    bool WriteToJson(JsonDocument& json, const char* name, uint64_t name_c);
    bool LoadFromJson(JsonDocument& json, const char* name);
    bool LoadFromJson(rapidjson::Value& json_overlay);
};

} // namespace LibTesix