#pragma once

#include "SegmentArray.h"
#include "Style.h"

#include <cinttypes>
#include <string>
#include <unicode/unistr.h>
#include <vector>

namespace LibTesix {

struct StyledString : public SegmentArray {
  public:
    StyledString(const icu::UnicodeString& base_string, const Style* style = style_allocator[0UL]);
    StyledString(const char* base_string, const Style* style = style_allocator[0UL]);

    StyledString(const SegmentArray& string);
    StyledString(const std::vector<SegmentArray::StyledSegment>& string);

    StyledString();

  public:
  public:
    void Insert(const icu::UnicodeString& str, const Style* style, uint64_t index);
    void Insert(const char* str, const Style* style, uint64_t index);
    void Append(const icu::UnicodeString& str, const Style* style);
    void Append(const char* str, const Style* style);
    void Erase(uint64_t start, uint64_t end);
    icu::UnicodeString Write(const icu::UnicodeString& str, const Style* style, uint64_t index);
    icu::UnicodeString Write(const char* str, const Style* style, uint64_t index);

    StyledString Substr(uint64_t start, uint64_t end) const;

    void Resize(uint64_t size);

    using SegmentArray::Len;

    void Clear(const Style* style = style_allocator[0UL]);
    void ClearStyle(const Style* style = style_allocator[0UL]);

    void UpdateRaw();
    std::string Raw(const Style& state, bool should_update = true);

#ifdef NDEBUG
    using SegmentArray::PrintDebug;
#endif

  private:
    std::string raw;

    void UpdateSegmentStart(uint64_t i = 0);
};

} // namespace LibTesix

// 1451174821