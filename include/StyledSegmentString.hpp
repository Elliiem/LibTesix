#pragma once

#define NDEBUG

#include "Style.hpp"

#include <cinttypes>
#include <numeric>
#include <unicode/unistr.h>

namespace LibTesix {

/**
 * @brief Stores a string with a styles. The string is split into segments which store their style,
 * content and the index where they start. Segments can never overlap so it is illegal to have a
 * segment at 0 with the string "foo" and a segment that starts at 2 because the first segment
 * occupies the second index since it is 3 characters long. This struct is used mostly internally,
 * you should use StyledString instead of this in the most cases. The difference between this and
 * StyledString is that this allows the string to have holes. The segments dont need to be after one
 * another so it would be legal to have a string that looks like this "foo**bar" (the stars represent
 * empty spaces in this example), this may seem to make no sense but it allows to handle the space
 * character. For example if I want to print this "foo**bar" to the screen the stars again are
 * empty spaces so they shouldnt affect what is already on the screen so if there was "########" on
 * the screen where I want to draw, the result would be "foo  bar" if we coudnt have holes in the
 * string, with holest the result would be "foo##bar" wich is very useful because non contiugous
 * data doesnt have to be stored in seperate objects.
 */
struct StyledSegmentString {
    /**
     * @brief A segment of the string. this is described in more detail in the description of
     * StyledSegmentString
     */
    struct Segment {
        Segment(const icu::UnicodeString& str, const Style& style, uint64_t start);
        Segment(const Segment&& seg);
        Segment(const Segment& seg);

        uint64_t           _start;
        const Style&       _style;
        icu::UnicodeString _str;

        /**
         * @brief Clones the segment
         * @returns A pointer to the clone
         */
        std::unique_ptr<Segment> Clone() const;
    };

    /**
     * @brief Stores a character with its style
     */
    struct StyledChar {
      public:
        UChar32      _character;
        const Style* _style;
    };

    StyledSegmentString() = default;
    StyledSegmentString(const StyledSegmentString& str);

    /**
     * @brief The use of pointers in _segments is necessitated by the fucking inability to use const Style& directly in segments.
     *        This is due to std::vector::erase requiring items to be assignable, which is fucking impossible with references,
     *        let alone constants. Therefore, the items are wrapped by a pointer, allowing them to be assigned instead
     *        of the items themselves. (EUDAAA!!!!)
     */
    std::vector<std::unique_ptr<Segment>> _segments;

  public:
    /**
     * @brief Appends a segment to the string
     */
    void Append(const icu::UnicodeString& str, const Style& style);

    /**
     * @brief Erases the string in the given range
     */
    void Erase(std::size_t start = 0, std::size_t end = std::numeric_limits<size_t>::max());

    /**
     * @brief Adds a segment at the index, overwrites other segments
     */
    void Add(const icu::UnicodeString& str, const Style& style, std::size_t index);

    /**
     * @brief Inserts a segment at the index, unlike Add moves following segments if there is no
     * space
     */
    void Insert(const icu::UnicodeString& str, const Style& style, std::size_t index);

    /**
     * @brief Gets the lenght of the string ie. the start of the last segment plus the lenght of its
     * string
     */
    inline uint64_t Len() const;

    // private:

    /**
     * @brief Finds the index of the segment containing the given index with binary search
     */
    inline std::size_t GetSegmentIndex(std::size_t index) const;

    /**
     * @brief Check if the index is in the string of the segment at the index.
     */
    inline bool IsInSegment(std::size_t index) const;

    /**
     * @brief Merges the first and second segment, this does no checks so the caller needs to make
     * sure everything is valid. The order of the segments is important as the string of the second
     * segment gets appended to the first, even though they might be the other way around.
     */
    inline void Merge(std::size_t first_index, std::size_t second_index);

    /**
     * @brief Inserts a segment to the string automatically merges if suitable.
     * The caller needs to make sure the segment is legal.
     */
    void InsertSegment(const icu::UnicodeString& str, const Style& style, uint64_t start, std::size_t index);

#ifdef NDEBUG
    /**
     * @brief Prints debug info of the string
     */
    void PrintDebug() const;
#endif
};

} // namespace LibTesix