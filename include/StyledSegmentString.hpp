#pragma once

#define NDEBUG

#include "Style.hpp"

#include <cinttypes>
#include <numeric>
#include <tinyutf8/tinyutf8.h>

namespace LibTesix {

/**
 * @brief Stores a styled string. Unlike StyledString this allows segments to be non-contiguos.
 * This is the base class of StyledString.
 */
class StyledSegmentString {
  public:
    /**
     * @brief A segment that stores information about a part of the string. For example, if we have the string "foobar"
     * and 'foo' is blue, and 'bar' is red, 'foo' would be a segment, and 'bar' would be a segment. So, in segments, the
     * string would look something like this: [{"foo", blue, 0}, {"bar", red, 3}]. The numbers signify the start or
     * index of the segment within the overall string. 'foo' starts at index 0, and 'bar' starts at 3. Segments have one
     * rule within StyledSegmentString: they cant overlap. Overlap means the start of one segment is within the
     * contents of another. For example, this would be illegal: [{"foo", blue, 0}, {"bar", red, 1}]; the start of 'bar'
     * could be >= 3 but not < 3 because 'foo' "occupies" these indices. StyledString further restricts segments; here,
     * they need to be contiguous. The segments need to be one after another; 'bar' here would need to start at 3. Any
     * less, they overlap; any more, they arent one after another, so there is a hole in the string.
     */
    struct Segment {
        /**
         * @brief Constructor of Segment
         * @param str The contents/string of the segment
         * @param style The style of the segment
         * @param start The start/index of the segment in the overall string
         */
        Segment(const tiny_utf8::string& str, const Style& style, uint64_t start);

        uint64_t          _start;
        const Style&      _style;
        tiny_utf8::string _str;
    };

    /**
     * @brief Stores a character with a style
     */
    struct StyledChar {
      public:
        StyledChar(char32_t character, const Style& style);

        char32_t     _char;
        const Style& _style;
    };

    struct Reference {
        friend StyledSegmentString;

      public:
        Reference& operator=(const StyledChar& character);
        Reference& operator=(const Reference& character);

        operator StyledChar();

      private:
        std::size_t          _index;
        StyledSegmentString& _src;

        Reference(StyledSegmentString& src, std::size_t index);
    };

    /**
     * @brief Default constructor of StyledSegmenString
     */
    StyledSegmentString() = default;

    /**
     * @brief Copy constructor of StyledSegmentString
     * @param str The string to copy
     */
    StyledSegmentString(const StyledSegmentString& str);

  protected:
    std::vector<std::unique_ptr<Segment>> _segments;

  public:
    /**
     * @brief Appends a segment to the string
     * @param str The contents of the new segment
     * @param style The style of the segment
     */
    void Append(const tiny_utf8::string& str, const Style& style);

    /**
     * @brief Erases the string in the given range
     * @param start The starting index(g) of the range
     * @param end The ending index(g) of the range
     */
    void Erase(std::size_t start = 0, std::size_t end = SIZE_MAX);

    /**
     * @brief Adds a segment at the index(g), overwrites other segments
     * @param str The contents of the new segment
     * @param style The style of the segment
     * @param index The index(g) where the segment should be inserted
     */
    void Add(const tiny_utf8::string& str, const Style& style, std::size_t index);

    /**
     * @brief Inserts a segment at the index(g), moves following segments back if there is no space
     * @param str The contents of the new segment
     * @param style The style of the segment
     * @param index The index(g) where the segment should be inserted
     */
    void Insert(const tiny_utf8::string& str, const Style& style, std::size_t index);

    /**
     * @brief Sets the character at index(g) to the provided one
     * @param character The new character
     * @param index The index(g) to replace
     */
    void Set(const StyledChar& character, std::size_t index);

    /**
     * @brief Gets the length of the string, i.e., the start of the last segment plus the length of its string.
     */
    uint64_t Len() const;

    Reference operator[](std::size_t index);

#ifdef NDEBUG
    /**
     * @brief Prints debug info of the string
     */
    void PrintDebug() const;
#endif

  protected:
    /**
     * @brief Finds the index of the segment containing the given index with binary search.
     * @param index The index to search for.
     * @return The index of the segment containing the given index.
     */
    std::size_t GetSegmentIndex(std::size_t index) const;

    /**
     * @brief Inserts a segment to the string automatically merges if suitable.
     * The caller needs to make sure the segment is legal.
     * @param str The contents of the new segment
     * @param style The style of the segment
     * @param start The start of the segment
     * @param index The index(s) where to insert
     * @returns Whether or not the inserted segment was inserted by merging
     */
    bool InsertSegment(const tiny_utf8::string& str, const Style& style, uint64_t start, std::size_t index);

    /**
     * @brief Inserts a segment to the string automatically merges if suitable.
     * The caller needs to make sure the segment is legal.
     * @param seg The new segment
     * @param index The index(s) where to insert
     * @returns Whether or not the inserted segment was inserted by merging
     */
    bool InsertSegment(std::unique_ptr<Segment> seg, std::size_t index);

    /**
     * @brief Moves a segment to the destination and merges if possible, doesnt check if the move is legal
     * @param segment_index The index of the segmnent
     * @param dest The destination
     * @returns Whether or not the inserted segment was moved by merging
     */
    bool MoveSegment(std::size_t segment_index, uint64_t dest);
};

} // namespace LibTesix