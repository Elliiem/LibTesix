#pragma once

#include "StyledString.h"

namespace LibTesix {
struct Overlay {
    //
  public:
    Overlay(uint32_t width, uint32_t height);

    std::vector<StyledSegmentArray> lines;

    uint32_t width;

    void PrintDebug();

    void Write(uint32_t x, uint32_t y, const icu::UnicodeString& str, Style style);
    bool HitsSegment(uint32_t x, uint32_t y, uint32_t len);
    bool InSegment(uint32_t segment_index, uint32_t index);
};
} // namespace LibTesix