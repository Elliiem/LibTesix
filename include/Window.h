#pragma once

#include "StyledString.h"

#include <vector>

namespace LibTesix {

class Window {
  public:
    Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    std::vector<StyledString> lines;

    uint32_t x;
    uint32_t y;

    uint32_t width;
    uint32_t height;

    uint32_t x_cutoff;
    uint32_t y_cutoff;

    std::string raw_string;

    void Print(uint32_t x, uint32_t y, std::string str, Style style);
    void Update();

    void Draw(Style* state, bool should_update);
};

} // namespace LibTesix