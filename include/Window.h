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

class Window {
  public:
  private:
    typedef std::pair<uint32_t, uint32_t> range;

  public:
    Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    void Write(uint32_t x, uint32_t y, icu::UnicodeString& str, Style style);
    void Write(uint32_t x, uint32_t y, const char* str, Style style);
    void UpdateRaw();

    void Draw(Style& state, bool should_update = true);

    uint32_t GetHeight();
    uint32_t GetWidth();

    int32_t GetX();
    int32_t GetY();

    void Move(int32_t x, int32_t y);
    void Resize(int32_t width, int32_t height);

    void ApplyOverlay(Overlay& overlay);
    void ApplyOverlay();
    void RemoveOverlay();

  private:
    std::vector<StyledString> lines;

    Overlay overlay;
    bool overlay_enabled;

    std::string raw;
    Style raw_start_style;
    Style raw_end_style;

  public:
    int32_t x;
    int32_t y;

    uint32_t width;
    uint32_t height;

  private:
    const range GetXVisible();
    const range GetYVisible();

    void ApplyOverlayToVisibleSubstr(uint32_t line, uint32_t visible_start, StyledString& visible);
};

} // namespace LibTesix