#pragma once

#include "../StyledSegmentString.hpp"

namespace LibTesix {

using _Segment    = StyledSegmentString::Segment;
using _SegmentPtr = std::unique_ptr<_Segment>;

template<typename T>
inline bool IsMergable(const tiny_utf8::string& str, const Style& style, const uint64_t start, const T& cmp) {
    throw std::runtime_error("Unsupported Type! << IsMergable()");
}
/**
 * @brief Checks if the Segment represented by str, style and start can be merged (in front of cmp)
 * with cmp
 */
template<> inline bool IsMergable<_Segment>(
    const tiny_utf8::string& str, const Style& style, const uint64_t start, const _Segment& cmp) {
    bool is_consective = cmp._start == start + str.length();
    bool is_same_style = cmp._style == style;
    bool is_empty      = cmp._str.empty();

    return is_consective && is_same_style || is_empty;
}

template<> inline bool IsMergable<_SegmentPtr>(
    const tiny_utf8::string& str, const Style& style, const uint64_t start, const _SegmentPtr& cmp) {
    bool is_consective = cmp->_start == start + str.length();
    bool is_same_style = cmp->_style == style;
    bool is_empty      = cmp->_str.empty();

    return is_consective && is_same_style || is_empty;
}

/**
 * @brief Checks if the Segment represented by str, style and start can be merged (after cmp) with
 * cmp
 */
template<typename T>
inline bool IsMergable(const T& cmp, const tiny_utf8::string& str, const Style& style, const uint64_t start) {
    throw std::runtime_error("Unsupported Type! << IsMergable()");
}

template<> inline bool IsMergable<_Segment>(
    const _Segment& cmp, const tiny_utf8::string& str, const Style& style, const uint64_t start) {
    bool is_consective = start == cmp._start + cmp._str.length();
    bool is_same_style = style == cmp._style;
    bool is_empty      = str.empty();

    return is_consective && is_same_style || is_empty;
}

template<> inline bool IsMergable<_SegmentPtr>(
    const _SegmentPtr& cmp, const tiny_utf8::string& str, const Style& style, const uint64_t start) {
    bool is_consective = start == cmp->_start + cmp->_str.length();
    bool is_same_style = style == cmp->_style;
    bool is_empty      = str.empty();

    return is_consective && is_same_style || is_empty;
}

/**
 * @brief Checks if the two segments can be merged (the second segment after the first)
 */
template<typename T> inline bool IsMergable(const T& first, const T& second) {
    throw std::runtime_error("Unsupported Type! << IsMergable()");
}

template<> inline bool IsMergable<_Segment>(const _Segment& first, const _Segment& second) {
    bool is_consecutive = second._start == first._start + first._str.length();
    bool is_same_style  = first._style == second._style;
    bool is_empty       = second._str.empty();

    return is_consecutive && is_same_style || is_empty;
}

template<> inline bool IsMergable<_SegmentPtr>(const _SegmentPtr& first, const _SegmentPtr& second) {
    bool is_consecutive = second->_start == first->_start + first->_str.length();
    bool is_same_style  = first->_style == second->_style;
    bool is_empty       = second->_str.empty();

    return is_consecutive && is_same_style || is_empty;
}

/**
 * @brief Checks if the index is within the bounds of the segment
 */
template<typename T> inline bool IsInSegment(std::size_t index, const T& segment) {
    throw std::runtime_error("Unsupported Type! << IsInSegment()");
}

template<> inline bool IsInSegment<_Segment>(std::size_t index, const _Segment& segment) {
    bool is_after_start = segment._start <= index;
    bool is_before_end  = index < segment._start + segment._str.length();

    return is_after_start && is_before_end;
}

template<> inline bool IsInSegment<_SegmentPtr>(std::size_t index, const _SegmentPtr& segment) {
    bool is_after_start = segment->_start <= index;
    bool is_before_end  = index < segment->_start + segment->_str.length();

    return is_after_start && is_before_end;
}

template<typename T> inline bool IsRangeInSegment(std::size_t start, std::size_t end, const T& seg) {
    throw std::runtime_error("Unsupported Type! << IsInSegment()");
}

template<> inline bool IsRangeInSegment<_Segment>(std::size_t start, std::size_t end, const _Segment& seg) {
    if(start > end) {
        std::swap(start, end);
    }

    bool after_start = start >= seg._start;
    bool before_end  = end < seg._start + seg._str.length();

    return after_start && before_end;
}

template<> inline bool IsRangeInSegment<_SegmentPtr>(std::size_t start, std::size_t end, const _SegmentPtr& seg) {
    bool after_start = start >= seg->_start;
    bool before_end  = end < seg->_start + seg->_str.length();

    return after_start && before_end;
}

/**
 * @brief Helper that creates a unique_ptr to a Segment with the parameters
 */
inline _SegmentPtr CreateSegment(const tiny_utf8::string& str, const Style& style, uint64_t start) {
    return std::make_unique<_Segment>(str, style, start);
}

template<typename T> inline _SegmentPtr SplitSegment(T& seg, std::size_t index, std::size_t erase_len) {
    throw std::runtime_error("Unsupported Type! << SplitSegment()");
}

template<> inline _SegmentPtr SplitSegment<_Segment>(_Segment& seg, std::size_t index, std::size_t erase_len) {
    if(index > seg._str.length()) {
        throw std::range_error("Index is out of Bounds! << SplitSegment()");
    }

    if(index == 0) {
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

template<> inline _SegmentPtr SplitSegment<_SegmentPtr>(_SegmentPtr& seg, std::size_t index, std::size_t erase_len) {
    if(index > seg->_str.length()) {
        throw std::range_error("Index is out of Bounds! << SplitSegment()");
    }

    if(index == 0) {
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
 * Erases a range within a segment
 */
template<typename T> inline void SegmentErase(T& seg, std::size_t index = 0, std::size_t len = SIZE_MAX) {
    throw std::runtime_error("Unsupported Type! << SegmentErase()");
}

template<> inline void SegmentErase<_Segment>(_Segment& seg, std::size_t index, std::size_t len) {
    if(index >= seg._str.length()) {
        throw std::range_error("Index is out of bounds! << SegmentErase()");
    }

    std::size_t old_len = seg._str.length();

    seg._str.erase(index, len);

    if(index == 0) {
        seg._start += index + len < old_len ? len : 0;
    }
}

template<> inline void SegmentErase<_SegmentPtr>(_SegmentPtr& seg, std::size_t index, std::size_t len) {
    if(index >= seg->_str.length()) {
        throw std::range_error("Index is out of bounds! << SegmentErase()");
    }

    std::size_t old_len = seg->_str.length();

    seg->_str.erase(index, len);

    if(index == 0) {
        seg->_start += index + len < old_len ? len : 0;
    }
}

/**
 * Replaces a part of a segment inplace
 */
template<typename T> inline void SegmentReplaceInplace(
    T& seg, const tiny_utf8::string& str, std::size_t index = 0, std::size_t len = SIZE_MAX) {
    throw std::runtime_error("Unsupported Type! << SegmentReplaceInplace()");
}

template<> inline void SegmentReplaceInplace<_Segment>(
    _Segment& seg, const tiny_utf8::string& str, std::size_t index, std::size_t len) {
    if(index >= seg._str.length()) {
        throw std::runtime_error("Index is out of bounds! <<  SegmentReplaceInplace()");
    }

    len = std::min(len, std::min(str.length(), seg._str.length() - index));

    for(uint64_t i = 0; i < len; i++) {
        seg._str[i + index] = str[i];
    }
}

template<> inline void SegmentReplaceInplace<_SegmentPtr>(
    _SegmentPtr& seg, const tiny_utf8::string& str, std::size_t index, std::size_t len) {
    if(index >= seg->_str.length()) {
        throw std::runtime_error("Index is out of bounds! <<  SegmentReplaceInplace()");
    }

    len = std::min(len, std::min(str.length(), seg->_str.length() - index));

    for(uint64_t i = 0; i < len; i++) {
        seg->_str[i + index] = str[i];
    }
}

} // namespace LibTesix