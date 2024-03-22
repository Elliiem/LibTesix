#pragma once

#include "../StyledString.hpp"

namespace LibTesix {

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
 * @brief Helper that creates a unique_ptr to a Segment with the parameters
 */
inline _SegmentPtr CreateSegment(const tiny_utf8::string& str, const Style& style, uint64_t start) {
    return std::make_unique<Segment>(str, style, start);
}

/**
 * @brief Helper that creates a unique_ptr to a Segment with the parameters
 */
inline _SegmentPtr CreateSegment(const StyledChar& character, uint64_t start) {
    return std::make_unique<Segment>(tiny_utf8::string(character._char), character._style, start);
}

template<typename T> inline std::size_t GetSegmentEnd(const T& segment) {
    throw std::runtime_error("Unsupported Type! << GetSegmentEnd()");
}

template<> inline std::size_t GetSegmentEnd<Segment>(const Segment& segment) {
    bool longer_than_zero = segment._str.length() > 0;
    return segment._start + segment._str.length() - longer_than_zero;
}

template<> inline std::size_t GetSegmentEnd<_SegmentPtr>(const _SegmentPtr& segment) {
    bool longer_than_zero = segment->_str.length() > 0;
    return segment->_start + segment->_str.length() - longer_than_zero;
}

inline std::size_t GetSegmentEnd(const tiny_utf8::string& str, uint64_t index) {
    return index + str.length() - 1;
}

/**
 * @brief Checks if a range is inside of a segment
 * @param start The start(g) of the range
 * @param end The end(g) of the range
 * @param segment The segment to check
 */
template<typename T> inline bool IsRangeInSegment(std::size_t start, std::size_t end, const T& segment) {
    throw std::runtime_error("Unsupported Type! << IsInSegment()");
}

/**
 * @brief Specialization of IsRangeInSegment for Segment
 */
template<> inline bool IsRangeInSegment<Segment>(std::size_t start, std::size_t end, const Segment& segment) {
    if(start > end) {
        std::swap(start, end);
    }

    bool after_start = start >= segment._start;
    bool before_end  = end < segment._start + segment._str.length();

    return after_start && before_end;
}

/**
 * @brief Specialization of IsRangeInSegment for std::unique_ptr<Segment>
 */
template<> inline bool IsRangeInSegment<_SegmentPtr>(std::size_t start, std::size_t end, const _SegmentPtr& segment) {
    bool after_start = start >= segment->_start;
    bool before_end  = end < segment->_start + segment->_str.length();

    return after_start && before_end;
}

/**
 * @brief Erases a range within a segment
 * @param index The index(l) where to start erasing
 * @param len How many characters should be deleted
 */
template<typename T> inline void SegmentErase(T& segment, std::size_t index = 0, std::size_t len = SIZE_MAX) {
    throw std::runtime_error("Unsupported Type! << SegmentErase()");
}

/**
 * @brief Specialization of SegmentErase for Segment
 */
template<> inline void SegmentErase<Segment>(Segment& segment, std::size_t index, std::size_t len) {
    if(index >= segment._str.length()) {
        throw std::range_error("Index is out of bounds! << SegmentErase()");
    }

    if(index == 0) {
        segment._start += index + len < segment._str.length() ? len : 0;
    }

    segment._str.erase(index, len);
}

/**
 * @brief Specialization of SegmentErase for std::unique_ptr<Segment>
 */
template<> inline void SegmentErase<_SegmentPtr>(_SegmentPtr& segment, std::size_t index, std::size_t len) {
    if(index >= segment->_str.length()) {
        throw std::range_error("Index is out of bounds! << SegmentErase()");
    }

    if(index == 0) {
        segment->_start += index + len < segment->_str.length() ? len : 0;
    }

    segment->_str.erase(index, len);
}

/**
 * @brief Split a segment at a specified index and erase a specified number of characters.
 *
 * This function splits the given segment at the specified index and erases a specified
 * number of characters. It returns a new segment containing the removed substring.
 *
 * @param segment The segment to be split.
 * @param index The index at which the segment should be split.
 * @param erase_len The number of characters to be erased from the original segment.
 * @return A new segment containing the removed substring.
 */
template<typename T> inline _SegmentPtr SplitSegment(T& segment, std::size_t index, std::size_t erase_len) {
    throw std::runtime_error("Unsupported Type! << SplitSegment()");
}

/**
 * @brief SpecialiZation of SplitSegment for Segment
 */
template<> inline _SegmentPtr SplitSegment<Segment>(Segment& segment, std::size_t index, std::size_t erase_len) {
    if(index > segment._str.length()) {
        throw std::range_error("Index is out of Bounds! << SplitSegment()");
    }

    erase_len = std::min(erase_len, segment._str.length());

    bool is_segment_start = index == 0;

    bool is_true_split_unnecessary = index + erase_len >= segment._str.length();

    if(is_segment_start) {
        SegmentErase(segment, 0, erase_len);

        return std::move(CreateSegment("", segment._style, 0));
    }

    if(is_true_split_unnecessary) {
        return std::move(CreateSegment("", segment._style, 0));
    }

    tiny_utf8::string sub = segment._str.substr(index + erase_len, SIZE_MAX);
    segment._str.erase(index, SIZE_MAX);

    return std::move(CreateSegment(sub, segment._style, segment._start + index + erase_len));
}

/**
 * @brief SpecialiZation of SplitSegment for std::unique_ptr<Segment>
 */
template<>
inline _SegmentPtr SplitSegment<_SegmentPtr>(_SegmentPtr& segment, std::size_t index, std::size_t erase_len) {
    if(index > segment->_str.length()) {
        throw std::range_error("Index is out of Bounds! << SplitSegment()");
    }

    erase_len = std::min(erase_len, segment->_str.length());

    bool is_segment_start = index == 0;

    bool is_true_split_unnecessary = index + erase_len >= segment->_str.length();

    if(is_segment_start) {
        SegmentErase(segment, 0, erase_len);

        return std::move(CreateSegment("", segment->_style, 0));
    }

    if(is_true_split_unnecessary) {
        return std::move(CreateSegment("", segment->_style, 0));
    }

    tiny_utf8::string sub = segment->_str.substr(index + erase_len, SIZE_MAX);
    segment->_str.erase(index, SIZE_MAX);

    return std::move(CreateSegment(sub, segment->_style, segment->_start + index + erase_len));
}

/**
 * @brief Replaces a part of a segment inplace
 * @param str The string to insert
 * @param index The index(l) where to insert
 * @param len How many characters of str should be copied
 */
template<typename T> inline void SegmentReplaceInplace(
    T& segment, const tiny_utf8::string& str, std::size_t index = 0, std::size_t len = SIZE_MAX) {
    throw std::runtime_error("Unsupported Type! << SegmentReplaceInplace()");
}

template<> inline void SegmentReplaceInplace<Segment>(
    Segment& segment, const tiny_utf8::string& str, std::size_t index, std::size_t len) {
    if(index >= segment._str.length()) {
        throw std::runtime_error("Index is out of bounds! <<  SegmentReplaceInplace()");
    }

    len = std::min(len, std::min(str.length(), segment._str.length() - index));

    for(uint64_t i = 0; i < len; i++) {
        segment._str[i + index] = str[i];
    }
}

template<> inline void SegmentReplaceInplace<_SegmentPtr>(
    _SegmentPtr& segment, const tiny_utf8::string& str, std::size_t index, std::size_t len) {
    if(index >= segment->_str.length()) {
        throw std::runtime_error("Index is out of bounds! <<  SegmentReplaceInplace()");
    }

    len = std::min(len, std::min(str.length(), segment->_str.length() - index));

    for(uint64_t i = 0; i < len; i++) {
        segment->_str[i + index] = str[i];
    }
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
 * @brief Gets the character at the index within the segment
 * @param segment The segment
 * @param index The index(l)
 * @return The character at index
 */
template<typename T> inline StyledChar SegmentCharAt(const T& segment, std::size_t index) {
    throw std::runtime_error("Unsupported Type! << CharAt()");
}

/**
 * @brief Specialization of CharAt for Segment
 */
template<> inline StyledChar SegmentCharAt<Segment>(const Segment& segment, std::size_t index) {
#ifndef LIBTESIX_NO_SAFETY
    if(index >= segment._str.length()) {
        throw std::runtime_error(
            "Index is out of bounds of Segment! May or may not be void in string context. << CharAt()");
    }
#endif

    return StyledChar(segment._str[index], segment._style);
}

/**
 * @brief Specialization of CharAt for std::unique_ptr<Segment>
 */
template<> inline StyledChar SegmentCharAt<_SegmentPtr>(const _SegmentPtr& segment, std::size_t index) {
#ifndef LIBTESIX_NO_SAFETY
    if(index >= segment->_str.length()) {
        throw std::runtime_error(
            "Index is out of bounds of Segment! May or may not be void in string context. << CharAt()");
    }
#endif

    return StyledChar(segment->_str[index], segment->_style);
}

bool IsNull(const StyledChar& character) {
    return character._char == 0;
}

} // namespace LibTesix