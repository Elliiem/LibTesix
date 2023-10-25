#pragma once

#include "Style.h"

#include <cinttypes>
#include <string>
#include <unicode/unistr.h>
#include <vector>

namespace LibTesix {

struct StyledString {
  public:
  private:
    struct StyledSegment {
        StyledSegment(icu::UnicodeString str, Style style, uint32_t start = 0);
        StyledSegment();
        icu::UnicodeString str;

        Style style;
        uint32_t start;

        StyledSegment Split(uint32_t index);
    };

  public:
    StyledString(icu::UnicodeString base_string, Style style = STANDARD_STYLE);
    StyledString(std::vector<StyledSegment> string);
    StyledString();

  public:
  public:
    void Insert(icu::UnicodeString& str, Style style, uint32_t index);
    void Append(icu::UnicodeString& str, Style style);
    void Erase(uint32_t start, uint32_t end);
    icu::UnicodeString Write(icu::UnicodeString& str, Style style, uint32_t index);

    StyledString Substr(uint32_t start, uint32_t end);

    uint32_t Len();

    void Resize(uint32_t size);

    void Clear(Style style = STANDARD_STYLE);
    void ClearStyle(Style style = STANDARD_STYLE);

    void UpdateRaw();
    const std::string& Raw(Style& state, bool should_update = true);

    Style StyleStart();
    Style StyleEnd();

    void Print(Style& state, bool should_update = true);

  private:
    std::vector<StyledSegment> string;

    std::string raw;

  private:
    void UpdateSegmentStart(uint32_t i = 0);
    void BoundsCheck(uint32_t index, std::string message);
    uint32_t GetSegmentIndex(uint32_t index);
};

} // namespace LibTesix