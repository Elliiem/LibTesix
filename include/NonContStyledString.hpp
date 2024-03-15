#pragma once

#include "Style.hpp"

#include <cinttypes>
#include <numeric>
#include <tinyutf8/tinyutf8.h>

namespace LibTesix {

/**
 * @brief A segment that stores information about a part of the string. For example, if we have the string "foobar"
 * and 'foo' is blue, and 'bar' is red, 'foo' would be a segment, and 'bar' would be a segment. So, in segments, the
 * string would look something like this: [{"foo", blue, 0}, {"bar", red, 3}]. The numbers signify the start or
 * index of the segment within the overall string. 'foo' starts at index 0, and 'bar' starts at 3. Segments have one
 * rule within NonContStyledString: they cant overlap. Overlap means the start of one segment is within the
 * contents of another. For example, this would be illegal: [{"foo", blue, 0}, {"bar", red, 1}]; the start of 'bar'
 * could be >= 3 but not < 3 because 'foo' "occupies" these indices. StyledString further restricts segments; here,
 * they need to be contiguous. The segments need to be one after another; 'bar' here would need to start at 3. Any
 * less, they overlap; any more, they arent one after another, so there is a hole in the string.
 */
struct Segment {
  public:
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

    bool operator==(const StyledChar& other) const;
};

/**
 * @brief NonContiguousStyledString, stores a string with styles with non contiguous segments.
 * StyledString doesnt inherit from this, but this is basically the base of it.
 */
class NonContStyledString {
  public:
    /**
     * @brief A reference to a character in a NonContStyledString. This is used in order to be able to set single
     * characters with [] syntax. For everything else it is recommended to use StyledChars directly.
     */
    struct Reference {
        friend NonContStyledString;

      public:
        std::size_t          _index;
        NonContStyledString& _src;

        /**
         * @brief Sets the character in the source to a character
         * @param character The character
         */
        void operator=(const StyledChar& character);

        /**
         * @brief Sets the character in the source to the value of a reference
         * @param ref The reference
         */
        void operator=(const Reference& ref);

        bool operator==(const StyledChar& character) const;
        bool operator==(const Reference& ref) const;

      private:
        /**
         * @brief Constructor of Reference
         * @param src The source of the reference
         * @param index The index of the reference
         */
        Reference(NonContStyledString& src, std::size_t index);
    };

    /**
     * @brief Default constructor of StyledSegmenString
     */
    NonContStyledString() = default;

    /**
     * @brief Copy constructor of NonContStyledString
     * @param str The string to copy
     */
    NonContStyledString(const NonContStyledString& str);

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
    void Erase(std::size_t start = 0, std::size_t len = SIZE_MAX);

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
     * @brief Sets the character at index(g) to the on referred to by the reference
     * @param ref The reference
     * @param index The index(g) to replace
     */
    void Set(const Reference& ref, std::size_t index);

    /**
     * @brief Gets the length of the string.
     * @returns The lenght (The end of the last segment + 1)
     */
    uint64_t Len() const;

    /**
     * @brief Gets a reference to the index
     * @param index The index(g)
     */
    Reference operator[](std::size_t index);

    /**
     * @brief Gets the character at the index, if there is none it returns a null character.
     * You can use the IsNull helper to check if a character is null alternatively just check if the _char member is
     * zero
     * @param index The index(g)
     * @param fallback The style that will be used when a null character is returned
     */
    StyledChar CharAt(std::size_t index, const Style& fallback) const;

#ifdef NDEBUG
    /**
     * @brief Prints debug info of the string
     */
    void PrintDebug() const;
#endif

  protected:
    /**
     * @brief Finds the index of the segment owning the given index with binary search. A segment owns a index if it is
     * after its start and before the next segments, the first segment owns everything prior to it
     * @param index The index to search for.
     * @returns The index of the segment owning the given index.
     */
    std::size_t GetSegmentIndex(std::size_t index) const;

    /**
     * @brief Inserts a segment to the string automatically merges if suitable.
     * Doesnt chack if the new segment is illegal.
     * @param str The contents of the new segment
     * @param style The style of the segment
     * @param start The start of the segment
     * @param index The index(s) where to insert
     * @returns Whether or not the inserted segment was inserted by merging
     */
    bool InsertSegment(const tiny_utf8::string& str, const Style& style, uint64_t start, std::size_t index);

    /**
     * @brief Inserts a segment to the string automatically merges if suitable.
     * Doesnt chack if the new segment is illegal.
     * @param seg The new segment
     * @param index The index(s) where to insert
     * @returns Whether or not the inserted segment was inserted by merging
     */
    bool InsertSegment(std::unique_ptr<Segment> seg, std::size_t index);

    /**
     * @brief Moves a segment to the destination and merges if possible.
     * Doesnt check if the new position is illegal.
     * @param segment_index The index of the segmnent
     * @param dest The destination
     * @returns Whether or not the inserted segment was moved by merging
     */
    bool MoveSegment(std::size_t segment_index, uint64_t dest);
};

} // namespace LibTesix