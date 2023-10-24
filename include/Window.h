#pragma once

#include "StyledString.h"

#include <unicode/unistr.h>
#include <vector>

namespace LibTesix {
uint32_t GetTerminalWidth();
uint32_t GetTerminalHeight();

class Window {
  public:
  private:
    typedef std::pair<uint32_t, uint32_t> interval;

  public:
    Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    std::vector<StyledString> lines;

    int32_t x;
    int32_t y;

    uint32_t width;
    uint32_t height;

    std::string raw;

    void Print(uint32_t x, uint32_t y, icu::UnicodeString& str, Style style);
    void UpdateRaw();

    void Draw(Style& state, bool should_update = true);

  private:
    Style raw_start_style;
    Style raw_end_style;

  private:
    interval GetXVisible();
    interval GetYVisible();
};

} // namespace LibTesix