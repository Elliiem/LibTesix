#pragma once

#include "Overlay.h"
#include "StyledString.h"

#include <unicode/unistr.h>
#include <vector>

// Overrides the size of the terminal to 211 colums and 41 lines
// #define TTY_SIZE_OVERRIDE

namespace LibTesix {

uint32_t GetTerminalWidth();
uint32_t GetTerminalHeight();

typedef std::pair<int32_t, int32_t> Range;

Range ClampRange(uint32_t max, Range range);

class Window {
  public:
    Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Style style = STANDARD_STYLE);

  public:
    void Draw(Style& state, bool should_update = true);

    void Write(uint32_t x, uint32_t y, icu::UnicodeString& str, Style style);
    void Write(uint32_t x, uint32_t y, const char* str, Style style);

    void ApplyOverlay(Overlay& overlay);
    void ApplyOverlay();
    void RemoveOverlay();

    void Move(int32_t x, int32_t y);
    void Resize(int32_t width, int32_t height);

    void Clear(Style style);

    void UpdateRaw();

    uint32_t GetHeight();
    uint32_t GetWidth();

    int32_t GetX();
    int32_t GetY();

  private:
    std::vector<StyledString> lines;

    Overlay overlay;
    bool overlay_enabled;

    std::string raw;

    Style raw_start_style;
    Style raw_end_style;

    int32_t x;
    int32_t y;

    uint32_t width;
    uint32_t height;

  private:
    void ApplyOverlayToVisibleSubstr(uint32_t line, uint32_t visible_start, StyledString& visible);
};

} // namespace LibTesix