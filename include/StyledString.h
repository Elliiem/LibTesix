#pragma once

#include "SegmentArray.h"
#include "Style.h"

#include <cinttypes>
#include <string>
#include <unicode/unistr.h>
#include <vector>

namespace LibTesix {

struct StyledString : public StyledSegmentArray {
  public:
    StyledString(const icu::UnicodeString& base_string, Style style = STANDARD_STYLE);
    StyledString(const char* base_string, Style style = STANDARD_STYLE);

    StyledString(const StyledSegmentArray& string);
    StyledString(const std::vector<StyledSegment>& string);

    StyledString();

  public:
  public:
    void Insert(const icu::UnicodeString& str, Style style, uint64_t index);
    void Insert(const char* str, Style style, uint64_t index);
    void Append(const icu::UnicodeString& str, Style style);
    void Append(const char* str, Style style);
    void Erase(uint64_t start, uint64_t end);
    icu::UnicodeString Write(const icu::UnicodeString& str, Style style, uint64_t index);
    icu::UnicodeString Write(const char* str, Style style, uint64_t index);

    StyledString Substr(uint64_t start, uint64_t end);

    void Resize(uint64_t size);

    using StyledSegmentArray::Len;

    void Clear(Style style = STANDARD_STYLE);
    void ClearStyle(Style style = STANDARD_STYLE);

    void UpdateRaw();
    std::string Raw(const Style& state, bool should_update = true);

    Style StyleStart() const;
    Style StyleEnd() const;

    void Print(Style& state, bool should_update = true);
    using StyledSegmentArray::PrintDebug;

  private:
    std::string raw;

    void UpdateSegmentStart(uint64_t i = 0);
};

} // namespace LibTesix

// 1451174821