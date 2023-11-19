#pragma once

#include "SegmentArray.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace LibTesix {
struct Overlay {
    Overlay();
    Overlay(uint32_t width, uint32_t height);

    void Clear();

    void Box(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Style style, const char* right = "┃", const char* left = "┃",
        const char* top = "━", const char* bottom = "━", const char* top_right = "┏", const char* top_left = "┓", const char* bottom_right = "┗",
        const char* bottom_left = "┛");

    void Box(Style style, const char* right = "┃", const char* left = "┃", const char* top = "━", const char* bottom = "━",
        const char* top_right = "┏", const char* top_left = "┓", const char* bottom_right = "┗", const char* bottom_left = "┛");

    std::vector<StyledSegmentArray> lines;

    uint32_t height;
    uint32_t width;

    void UpdateWidth();
};

// TODO Create JSON.h and move to it
Overlay ReadOverlay(rapidjson::Document& json, const char* name);

rapidjson::Document OpenJson(const char* filename);
void SaveJson(rapidjson::Document& json, const char* filename);

void WriteOverlay(Overlay& overlay, std::string& name, rapidjson::Document& json);

} // namespace LibTesix