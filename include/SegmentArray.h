#pragma once

#include "Style.h"

#include <cinttypes>
#include <unicode/unistr.h>

namespace LibTesix {

struct StyledSegment {
    StyledSegment(const icu::UnicodeString& str, Style style, uint64_t start = 0);
    StyledSegment(const char* str, Style style, uint64_t start);
    StyledSegment();
    icu::UnicodeString str;

    Style style;
    uint64_t start;

    StyledSegment Split(uint64_t index);
    uint64_t Len() const;
};

struct StyledSegmentArray {
  public:
    StyledSegmentArray();

  public:
    std::vector<StyledSegment> segments;

  public:
    void Append(const icu::UnicodeString& str, Style style);
    void Append(const char* str, Style style);

    void Add(const icu::UnicodeString& str, const Style& style, uint64_t index);
    void Add(const char* str, const Style& style, uint64_t index);

    void Erase(uint64_t start, uint64_t end);

    void Clear();

    uint64_t Len() const;

    void PrintDebug() const;

  protected:
    uint64_t GetSegmentIndex(uint64_t index) const;
    void InsertSegment(StyledSegment segment, uint64_t index);

  private:
    bool InSegment(uint64_t segment_index, uint64_t index) const;
    bool Clean(uint64_t index);
    bool HitsSegment(uint64_t start, uint64_t end) const;
};
} // namespace LibTesix