#pragma once

#include "SegmentArray.h"

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
};
} // namespace LibTesix