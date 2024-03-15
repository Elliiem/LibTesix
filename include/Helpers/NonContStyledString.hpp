#pragma once

#include "../NonContStyledString.hpp"

namespace LibTesix {

using _SegmentPtr = std::unique_ptr<Segment>;

/**
 * @brief Checks if the Segment represented by str, style and start can be merged to the start of cmp
 * @param str The string of the segment
 * @param style The style of the segment
 * @param start The start(g) of the segment
 * @param cmp The segment to compare to (Segment|std::unique_ptr<Segment>)
 */
template<typename T>
inline bool IsMergable(const tiny_utf8::string& str, const Style& style, const uint64_t start, const T& cmp) {
    throw std::runtime_error("Unsupported Type! << IsMergable()");
}

/**
 * @brief Specialization of IsMergable (in front) for Segment
 */
template<> inline bool IsMergable<Segment>(
    const tiny_utf8::string& str, const Style& style, const uint64_t start, const Segment& cmp) {
    bool is_consective = cmp._start == start + str.length();
    bool is_same_style = cmp._style == style;
    bool is_empty      = cmp._str.empty();

    return is_consective && is_same_style || is_empty;
}

/**
 * @brief Specialization of IsMergable (in front) for std::unique_ptr<Segment>
 */
template<> inline bool IsMergable<_SegmentPtr>(
    const tiny_utf8::string& str, const Style& style, const uint64_t start, const _SegmentPtr& cmp) {
    bool is_consective = cmp->_start == start + str.length();
    bool is_same_style = cmp->_style == style;
    bool is_empty      = cmp->_str.empty();

    return is_consective && is_same_style || is_empty;
}

/**
 * @brief Checks if the Segment represented by str, style and start can be merged to the end of cmp
 * @param str The string of the segment
 * @param style The style of the segment
 * @param start The start(g) of the segment
 * @param cmp The segment to compare to (Segment|std::unique_ptr<Segment>)
 */
template<typename T>
inline bool IsMergable(const T& cmp, const tiny_utf8::string& str, const Style& style, const uint64_t start) {
    throw std::runtime_error("Unsupported Type! << IsMergable()");
}

/**
 * @brief Specialization of IsMergable (behind) for Segment
 */
template<> inline bool IsMergable<Segment>(
    const Segment& cmp, const tiny_utf8::string& str, const Style& style, const uint64_t start) {
    bool is_consective = start == cmp._start + cmp._str.length();
    bool is_same_style = style == cmp._style;
    bool is_empty      = str.empty();

    return is_consective && is_same_style || is_empty;
}

/**
 * @brief Specialization of IsMergable (behind) for std::unique_ptr<Segment>
 */
template<> inline bool IsMergable<_SegmentPtr>(
    const _SegmentPtr& cmp, const tiny_utf8::string& str, const Style& style, const uint64_t start) {
    bool is_consective = start == cmp->_start + cmp->_str.length();
    bool is_same_style = style == cmp->_style;
    bool is_empty      = str.empty();

    return is_consective && is_same_style || is_empty;
}

/**
 * @brief Checks if the two segments can be merged (the second segment after the first)
 * @param first The first segment
 * @param second The first second
 */
template<typename T> inline bool IsMergable(const T& first, const T& second) {
    throw std::runtime_error("Unsupported Type! << IsMergable()");
}

/**
 * @brief Specialization of IsMergable (Segments) for Segment
 */
template<> inline bool IsMergable<Segment>(const Segment& first, const Segment& second) {
    bool is_consecutive = second._start == first._start + first._str.length();
    bool is_same_style  = first._style == second._style;
    bool is_empty       = second._str.empty();

    return is_consecutive && is_same_style || is_empty;
}

/**
 * @brief Specialization of IsMergable (Segments) for std::unique_ptr<Segment>
 */
template<> inline bool IsMergable<_SegmentPtr>(const _SegmentPtr& first, const _SegmentPtr& second) {
    bool is_consecutive = second->_start == first->_start + first->_str.length();
    bool is_same_style  = first->_style == second->_style;
    bool is_empty       = second->_str.empty();

    return is_consecutive && is_same_style || is_empty;
}

/**
 * @brief Checks if the index(g) is within the bounds of the segment
 * @param index The index(g) to check
 * @param segment The segment to check
 */
template<typename T> inline bool IsInSegment(std::size_t index, const T& segment) {
    throw std::runtime_error("Unsupported Type! << IsInSegment()");
}

/**
 * @brief Specialization of IsInSegment for Segment
 */
template<> inline bool IsInSegment<Segment>(std::size_t index, const Segment& segment) {
    bool is_after_start = segment._start <= index;
    bool is_before_end  = index < segment._start + segment._str.length();

    return is_after_start && is_before_end;
}

/**
 * @brief Specialization of IsInSegment for std::unique_ptr<Segment>
 */
template<> inline bool IsInSegment<_SegmentPtr>(std::size_t index, const _SegmentPtr& segment) {
    bool is_after_start = segment->_start <= index;
    bool is_before_end  = index < segment->_start + segment->_str.length();

    return is_after_start && is_before_end;
}

/**
 * @brief Checks if a range is inside of a segment
 * @param start The start(g) of the range
 * @param end The end(g) of the range
 * @param seg The segment to check
 */
template<typename T> inline bool IsRangeInSegment(std::size_t start, std::size_t end, const T& seg) {
    throw std::runtime_error("Unsupported Type! << IsInSegment()");
}

/**
 * @brief Specialization of IsRangeInSegment for Segment
 */
template<> inline bool IsRangeInSegment<Segment>(std::size_t start, std::size_t end, const Segment& seg) {
    if(start > end) {
        std::swap(start, end);
    }

    bool after_start = start >= seg._start;
    bool before_end  = end < seg._start + seg._str.length();

    return after_start && before_end;
}

/**
 * @brief Specialization of IsRangeInSegment for std::unique_ptr<Segment>
 */
template<> inline bool IsRangeInSegment<_SegmentPtr>(std::size_t start, std::size_t end, const _SegmentPtr& seg) {
    bool after_start = start >= seg->_start;
    bool before_end  = end < seg->_start + seg->_str.length();

    return after_start && before_end;
}

/**
 * @brief Helper that creates a unique_ptr to a Segment with the parameters
 */
inline _SegmentPtr CreateSegment(const tiny_utf8::string& str, const Style& style, uint64_t start) {
    return std::make_unique<Segment>(str, style, start);
}

template<typename T> inline std::size_t GetSegmentEnd(const T& seg) {
    throw std::runtime_error("Unsupported Type! << GetSegmentEnd()");
}

template<> inline std::size_t GetSegmentEnd<Segment>(const Segment& seg) {
    bool longer_than_zero = seg._str.length() > 0;
    return seg._start + seg._str.length() - longer_than_zero;
}

template<> inline std::size_t GetSegmentEnd<_SegmentPtr>(const _SegmentPtr& seg) {
    bool longer_than_zero = seg->_str.length() > 0;
    return seg->_start + seg->_str.length() - longer_than_zero;
}

/**
 * @brief Split a segment at a specified index and erase a specified number of characters.
 *
 * This function splits the given segment at the specified index and erases a specified
 * number of characters. It returns a new segment containing the removed substring.
 *
 * @param seg The segment to be split.
 * @param index The index at which the segment should be split.
 * @param erase_len The number of characters to be erased from the original segment.
 * @return A new segment containing the removed substring.
 */
template<typename T> inline _SegmentPtr SplitSegment(T& seg, std::size_t index, std::size_t erase_len) {
    throw std::runtime_error("Unsupported Type! << SplitSegment()");
}

/**
 * @brief SpecialiZation of SplitSegment for Segment
 */
template<> inline _SegmentPtr SplitSegment<Segment>(Segment& seg, std::size_t index, std::size_t erase_len) {
#ifndef LIBTESIX_NO_SAFETY
    if(index > seg._str.length()) {
        throw std::range_error("Index is out of Bounds! << SplitSegment()");
    }
#endif

    if(index == 0) {
        seg._start = std::min(GetSegmentEnd(seg), seg._start + erase_len);
        seg._str.erase(0, erase_len);

        return std::move(CreateSegment("", seg._style, 0));
    } else {
        bool is_greater_than_remaining = erase_len > seg._str.length() - index;

        uint64_t sub_start = is_greater_than_remaining ? seg._str.length() : erase_len + index;

        tiny_utf8::string sub = seg._str.substr(sub_start, SIZE_MAX);

        seg._str.erase(index, SIZE_MAX);

        return std::move(CreateSegment(sub, seg._style, seg._start + sub_start));
    }
}

/**
 * @brief SpecialiZation of SplitSegment for std::unique_ptr<Segment>
 */
template<> inline _SegmentPtr SplitSegment<_SegmentPtr>(_SegmentPtr& seg, std::size_t index, std::size_t erase_len) {
#ifndef LIBTESIX_NO_SAFETY
    if(index > seg->_str.length()) {
        throw std::range_error("Index is out of Bounds! << SplitSegment()");
    }
#endif

    if(index == 0) {
        seg->_start = std::min(GetSegmentEnd(seg), seg->_start + erase_len);
        seg->_str.erase(0, erase_len);

        return std::move(CreateSegment("", seg->_style, 0));
    } else {
        bool is_greater_than_remaining = erase_len > seg->_str.length() - index;

        uint64_t sub_start = is_greater_than_remaining ? seg->_str.length() : erase_len + index;

        tiny_utf8::string sub = seg->_str.substr(sub_start, SIZE_MAX);

        seg->_str.erase(index, SIZE_MAX);

        return std::move(CreateSegment(sub, seg->_style, seg->_start + sub_start));
    }
}

/**
 * @brief Erases a range within a segment
 * @param index The index(l) where to start erasing
 * @param len How many characters should be deleted
 */
template<typename T> inline void SegmentErase(T& seg, std::size_t index = 0, std::size_t len = SIZE_MAX) {
    throw std::runtime_error("Unsupported Type! << SegmentErase()");
}

/**
 * @brief Specialization of SegmentErase for Segment
 */
template<> inline void SegmentErase<Segment>(Segment& seg, std::size_t index, std::size_t len) {
#ifndef LIBTESIX_NO_SAFETY
    if(index >= seg._str.length()) {
        throw std::range_error("Index is out of bounds! << SegmentErase()");
    }
#endif

    std::size_t old_len = seg._str.length();

    seg._str.erase(index, len);

    if(index == 0) {
        seg._start += index + len < old_len ? len : 0;
    }
}

/**
 * @brief Specialization of SegmentErase for std::unique_ptr<Segment>
 */
template<> inline void SegmentErase<_SegmentPtr>(_SegmentPtr& seg, std::size_t index, std::size_t len) {
#ifndef LIBTESIX_NO_SAFETY
    if(index >= seg->_str.length()) {
        throw std::range_error("Index is out of bounds! << SegmentErase()");
    }
#endif

    std::size_t old_len = seg->_str.length();

    seg->_str.erase(index, len);

    if(index == 0) {
        seg->_start += index + len < old_len ? len : 0;
    }
}

/**
 * @brief Replaces a part of a segment inplace
 * @param str The string to insert
 * @param index The index(l) where to insert
 * @param len How many characters of str should be copied
 */
template<typename T> inline void SegmentReplaceInplace(
    T& seg, const tiny_utf8::string& str, std::size_t index = 0, std::size_t len = SIZE_MAX) {
    throw std::runtime_error("Unsupported Type! << SegmentReplaceInplace()");
}

/**
 * @brief Specialization of SegmentReplaceInplace for Segment
 */
template<> inline void SegmentReplaceInplace<Segment>(
    Segment& seg, const tiny_utf8::string& str, std::size_t index, std::size_t len) {
#ifndef LIBTESIX_NO_SAFETY
    if(index >= seg._str.length()) {
        throw std::runtime_error("Index is out of bounds! <<  SegmentReplaceInplace()");
    }
#endif

    len = std::min(len, std::min(str.length(), seg._str.length() - index));

    for(uint64_t i = 0; i < len; i++) {
        seg._str[i + index] = str[i];
    }
}

/**
 * @brief Specialization of SegmentReplaceInplace for std::unique_ptr<Segment>
 */
template<> inline void SegmentReplaceInplace<_SegmentPtr>(
    _SegmentPtr& seg, const tiny_utf8::string& str, std::size_t index, std::size_t len) {
#ifndef LIBTESIX_NO_SAFETY
    if(index >= seg->_str.length()) {
        throw std::runtime_error("Index is out of bounds! <<  SegmentReplaceInplace()");
    }
#endif

    len = std::min(len, std::min(str.length(), seg->_str.length() - index));

    for(uint64_t i = 0; i < len; i++) {
        seg->_str[i + index] = str[i];
    }
}

/**
 * @brief Gets the character at the index within the segment
 * @param seg The segment
 * @param index The index(l)
 * @return The character at index
 */
template<typename T> inline StyledChar SegmentCharAt(const T& seg, std::size_t index) {
    throw std::runtime_error("Unsupported Type! << CharAt()");
}

/**
 * @brief Specialization of CharAt for Segment
 */
template<> inline StyledChar SegmentCharAt<Segment>(const Segment& seg, std::size_t index) {
#ifndef LIBTESIX_NO_SAFETY
    if(index >= seg._str.length()) {
        throw std::runtime_error(
            "Index is out of bounds of Segment! May or may not be void in string context. << CharAt()");
    }
#endif

    return StyledChar(seg._str[index], seg._style);
}

/**
 * @brief Specialization of CharAt for std::unique_ptr<Segment>
 */
template<> inline StyledChar SegmentCharAt<_SegmentPtr>(const _SegmentPtr& seg, std::size_t index) {
#ifndef LIBTESIX_NO_SAFETY
    if(index >= seg->_str.length()) {
        throw std::runtime_error(
            "Index is out of bounds of Segment! May or may not be void in string context. << CharAt()");
    }
#endif

    return StyledChar(seg->_str[index], seg->_style);
}

bool IsNull(const StyledChar& character) {
    return character._char == 0;
}

} // namespace LibTesix