#pragma once

#include "Style.h"

#include <cinttypes>
#include <string>
#include <unicode/unistr.h>
#include <vector>

namespace LibTesix {

struct StyledSegment {
    StyledSegment(const icu::UnicodeString& str, Style style, uint32_t start = 0);
    StyledSegment(const char* str, Style style, uint32_t start);
    StyledSegment();
    icu::UnicodeString str;

    Style style;
    uint32_t start;

    StyledSegment Split(uint32_t index);
};

struct SSA {
    SSA(const std::vector<StyledSegment>& string);
    SSA();

    std::vector<StyledSegment> string;

    uint32_t GetSegmentIndex(uint32_t index);
};

struct StyledString {
  public:
    StyledString(const icu::UnicodeString& base_string, Style style = STANDARD_STYLE);
    StyledString(const char* base_string, Style style = STANDARD_STYLE);

    StyledString(const SSA& string);
    StyledString(const std::vector<StyledSegment>& string);

    StyledString();

  public:
  public:
    void Insert(const icu::UnicodeString& str, Style style, uint32_t index);
    void Insert(const char* str, Style style, uint32_t index);
    void Append(const icu::UnicodeString& str, Style style);
    void Append(const char* str, Style style);
    void Erase(uint32_t start, uint32_t end);
    icu::UnicodeString Write(const icu::UnicodeString& str, Style style, uint32_t index);
    icu::UnicodeString Write(const char* str, Style style, uint32_t index);

    StyledString Substr(uint32_t start, uint32_t end);

    uint32_t Len();

    void Resize(uint32_t size);

    void Clear(Style style = STANDARD_STYLE);
    void ClearStyle(Style style = STANDARD_STYLE);

    void UpdateRaw();
    const std::string& Raw(const Style& state, bool should_update = true);

    Style StyleStart();
    Style StyleEnd();

    void Print(Style& state, bool should_update = true);

  private:
    SSA segments;

    std::string raw;

  private:
    void UpdateSegmentStart(uint32_t i = 0);
    void BoundsCheck(uint32_t index, std::string message);
};

} // namespace LibTesix

// 1451174821