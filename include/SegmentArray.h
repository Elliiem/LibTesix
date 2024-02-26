#pragma once

#define NDEBUG

#include "Style.h"

#include <cinttypes>
#include <optional>
#include <unicode/unistr.h>

namespace LibTesix {

struct SegmentArray {
  public:
    struct StyledSegment {
        StyledSegment(const icu::UnicodeString& str, const Style* style, uint64_t start = 0);
        StyledSegment(const char* str, const Style* style, uint64_t start);
        StyledSegment();

        icu::UnicodeString _str;

        const Style* _style;
        uint64_t _start;

        StyledSegment Split(uint64_t index);
        uint64_t Len() const;
    };

    struct StyledChar {
        StyledChar(char32_t character, const Style* style);

      public:
        char32_t _character;
        const Style* _style;

        bool operator==(const StyledChar& other) const;
    };

  public:
    SegmentArray();

  public:
    std::vector<StyledSegment> _segments;

  public:
    /**
     * @brief Appends a segment to the string
     * @param str The string to append
     * @param style The style of the string
     */
    void Append(const icu::UnicodeString& str, const Style* style);
    void Append(const char* str, const Style* style);

    /**
     * @brief Adds a string to the string at the index. Overwrites any of the prexisting string.
     * This can also be outside of the bounds of the string, it will expand the string.
     * @param str The string to add
     * @param style The style of the string
     * @param index The index at which to add the string
     */
    void Add(const icu::UnicodeString& str, const Style* style, uint64_t index);
    void Add(const char* str, const Style* style, uint64_t index);

    /**
     * @brief Adds a StyledChar to the string at the index. Overwrites any of the prexisting string.
     * This can also be outside of the bounds of the string, it will expand the string.
     * @param character The StyledChar to add
     * @param index The index at which to add the string
     */
    void Add(const StyledChar& character, const uint64_t index);

    /**
     * @brief Erases the string in a range, The erased space is left empty
     * @param start The start of the range
     * @param end The end of the range
     */
    void Erase(const uint64_t start, const uint64_t end);

    /**
     * @brief Clears the string
     */
    void Clear();

    /**
     * @brief Gets the length of the string
     * @returns The lenght
     */
    uint64_t Len() const;

    /**
     * @brief Merges the segments in a given range if they are mergable ie. if their style is the same
     * else it does nothing. If there are multiple areas that can be merged they will be, so if start
     * and end are the first and last segment the whole string will be merged
     * @param start The start of the range
     * @param end The end of the range
     */
    void Merge(uint64_t start, uint64_t end = UINT64_MAX);

#ifdef NDEBUG
    void PrintDebug() const;
#endif

    StyledChar operator[](uint64_t index) const;

    const Style* GetStyleStart() const;
    const Style* GetStyleEnd() const;

  protected:
    uint64_t GetSegmentIndex(uint64_t index) const;
    void InsertSegment(StyledSegment segment, uint64_t index);

  private:
    bool InSegment(uint64_t segment_index, uint64_t index) const;
    bool Clean(uint64_t index);
    bool HitsSegment(uint64_t start, uint64_t end) const;
};

} // namespace LibTesix