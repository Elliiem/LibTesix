#pragma once

#include "Style.h"

#include <cinttypes>
#include <string>
#include <vector>

namespace LibTesix {

struct StyledString {
  public:
  private:
    struct StyledSegment {
        StyledSegment(std::string str, Style style, uint32_t start = 0);
        std::string str;
        Style style;
        uint32_t start;

        StyledSegment Split(uint32_t index);
        static StyledSegment Combine(std::vector<StyledSegment> segments);
    };

  public:
    StyledString(std::string base_string);
    StyledString(std::vector<StyledSegment> string);

  public:
  public:
    void Insert(std::string str, Style style, uint32_t index);
    void Append(std::string str, Style style);
    void Erase(uint32_t start, uint32_t end);
    std::string Write(std::string str, Style style, uint32_t index);

    uint32_t Len();

    void Resize(uint32_t size);

    void Clear(Style style = STANDARD_STYLE);
    void ClearStyle(Style style = STANDARD_STYLE);

    void UpdateRaw();
    std::string& Raw(Style* state);

    Style StyleStart();
    Style StyleEnd();

    StyledString Substr(uint32_t start, uint32_t end);

  private:
    std::vector<StyledSegment> string;

    std::string raw_string;

  private:
    void UpdateSegmentStart();
    uint32_t GetSegmentIndex(uint32_t index);
    std::pair<uint32_t, uint32_t> Split(uint32_t index);
};

} // namespace LibTesix