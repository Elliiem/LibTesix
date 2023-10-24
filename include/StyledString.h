#pragma once

#include "Style.h"

#include <cinttypes>
#include <string>
#include <unicode/ubrk.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/utext.h>
#include <unicode/utypes.h>
#include <vector>

namespace LibTesix {

uint32_t StringLenght(std::string& str);

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

  public:
  public:
    void Insert(std::string str, Style style, uint32_t index);
    void Append(std::string str, Style style);
    void Erase(uint32_t start, uint32_t end);
    std::string Write(std::string str, Style style, uint32_t index);

    StyledString Substr(uint32_t start, uint32_t end);

    uint32_t Len();

    void Resize(uint32_t size);

    void Clear(Style style = STANDARD_STYLE);
    void ClearStyle(Style style = STANDARD_STYLE);

    void UpdateRaw();
    std::string& Raw(Style& state);

    Style StyleStart();
    Style StyleEnd();

    void Print(Style& state);

    void Dev();

  private:
    std::vector<StyledSegment> string;

    std::string raw;
    icu::UnicodeString raw_str;

  private:
    // TODO Remove when done debuging
    void PrintDebug();

    void UpdateSegmentStart(uint32_t i = 0);
    void BoundsCheck(uint32_t index, std::string message);
    uint32_t GetSegmentIndex(uint32_t index);
};

} // namespace LibTesix