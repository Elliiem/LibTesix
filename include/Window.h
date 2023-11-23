#pragma once

#include "Json.h"
#include "Overlay.h"
#include "StyledString.h"

#include <unicode/unistr.h>
#include <vector>

namespace LibTesix {

typedef std::pair<int64_t, int64_t> Range;

Range ClampRange(uint64_t max, Range range);

void ApplySegmentArray(StyledSegmentArray& arr, StyledString& str, uint64_t offset = 0);

class Window {
  public:
    Window(int64_t x, int64_t y, uint64_t width, uint64_t height, Style style = STANDARD_STYLE);
    Window(JsonDocument& json, const char* name);
    Window(rapidjson::Value& json_window);

  public:
    void Draw(Style& state, bool should_update = true);

    void Write(uint64_t col, uint64_t line, icu::UnicodeString& str, Style style);
    void Write(uint64_t col, uint64_t line, const char* str, Style style);

    void ApplyOverlay(Overlay& overlay);
    void ApplyOverlay();
    void RemoveOverlay();

    void Move(int64_t x, int64_t y);
    void Resize(uint64_t width, uint64_t height);

    void Clear(Style style);

    void UpdateRaw();

    uint64_t GetHeight();
    uint64_t GetWidth();

    int64_t GetX();
    int64_t GetY();

    bool WriteToJson(JsonDocument& json, const char* name, uint64_t name_c);
    bool LoadFromJson(JsonDocument& json, const char* name);
    bool LoadFromJson(rapidjson::Value& json_window);

  private:
    std::vector<StyledString> lines;

    Overlay overlay;
    bool overlay_enabled = false;
    bool has_overlay = false;

    std::string raw;

    Style raw_start_style;
    Style raw_end_style;

    int64_t x;
    int64_t y;

    uint64_t width;
    uint64_t height;
};

} // namespace LibTesix
