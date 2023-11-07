#pragma once

#include "Style.h"

#include <cinttypes>
#include <unicode/unistr.h>

namespace LibTesix {

struct StyledSegment {
    StyledSegment(const icu::UnicodeString& str, Style style, uint32_t start = 0);
    StyledSegment(const char* str, Style style, uint32_t start);
    StyledSegment();
    icu::UnicodeString str;

    Style style;
    uint32_t start;

    StyledSegment Split(uint32_t index);
    uint32_t Len();
};

struct StyledSegmentArray {
    StyledSegmentArray();

    std::vector<StyledSegment> segments;

    bool InSegment(uint32_t segment_index, uint32_t index);

    void Append(const icu::UnicodeString& str, Style style);
    void Append(const char* str, Style style);

    void Add(const icu::UnicodeString& str, Style style, uint32_t index);
    void Add(const char* str, Style style, uint32_t index);

    void Erase(uint32_t start, uint32_t end);

    void Clear();

    void InsertSegment(StyledSegment segment, uint32_t index);
    void PrintDebug();
    uint32_t Len();
    uint32_t GetSegmentIndex(uint32_t index);
    bool Clean(uint32_t index);
    bool HitsSegment(uint32_t start, uint32_t end);
};
} // namespace LibTesix