#pragma once

#include "StyledString.h"

namespace LibTesix {
struct StyledSegmentArray {
    StyledSegmentArray();

    std::vector<StyledSegment> segments;

    bool InSegment(uint32_t segment_index, uint32_t index);

    void Append(const icu::UnicodeString& str, Style style);
    void Append(const char* str, Style style);

    void Insert(const icu::UnicodeString& str, Style style, uint32_t index);
    void Insert(const char* str, Style style, uint32_t index);

    void Erase(uint32_t start, uint32_t end);

    void InsertSegment(StyledSegment segment, uint32_t index);
    void PrintDebug();
    uint32_t Len();
    uint32_t GetSegmentIndex(uint32_t index);
    bool Clean(uint32_t index);
};
} // namespace LibTesix