#include "StyledSegmentString.h"

namespace LibTesix {

template<typename> struct is_segment_trait : std::false_type {};
template<> struct is_segment_trait<StyledSegmentString::Segment> : std::true_type {};

template<typename T> inline constexpr bool is_segment = is_segment_trait<std::remove_const_t<T>>::value;

template<typename> struct is_segment_ptr_trait : std::false_type {};
template<> struct is_segment_ptr_trait<std::unique_ptr<StyledSegmentString::Segment>> : std::true_type {};
template<> struct is_segment_ptr_trait<std::unique_ptr<const StyledSegmentString::Segment>> : std::true_type {};

template<typename T> inline constexpr bool is_segment_ptr = is_segment_ptr_trait<std::remove_const_t<T>>::value;

/**
 * @brief Checks if the Segment represented by str, style and start can be merged (in front of cmp) with cmp
 */
template<typename T>
inline bool IsMergable(const icu::UnicodeString& str, const Style& style, const uint64_t start, const T& cmp) {
    bool is_consective = false;
    bool is_same_style = false;
    bool is_empty      = false;

    if constexpr(is_segment_ptr<T>) {
        is_consective = cmp->_start == start + str.length();
        is_same_style = cmp->_style == style;
        is_empty      = cmp->_str.isEmpty();
    } else if constexpr(is_segment<T>) {
        is_consective = cmp._start == start + str.length();
        is_same_style = cmp._style == style;
        is_empty      = cmp._str.isEmpty();
    }

    return is_consective && is_same_style || is_empty;
}

/**
 * @brief Checks if the Segment represented by str, style and start can be merged (after cmp) with cmp
 */
template<typename T>
inline bool IsMergable(const T& cmp, const icu::UnicodeString& str, const Style& style, const uint64_t start) {
    bool is_consective = false;
    bool is_same_style = false;
    bool is_empty      = false;

    if constexpr(is_segment_ptr<T>) {
        is_consective = start == cmp->_start + cmp->_str.length();
        is_same_style = style == cmp->_style;
        is_empty      = str.isEmpty();
    } else if constexpr(is_segment<T>) {
        is_consective = start == cmp._start + cmp._str.length();
        is_same_style = style == cmp._style;
        is_empty      = str.isEmpty();
    }

    return is_consective && is_same_style || is_empty;
}

/**
 * @brief Checks if the two segments can be merged (the second segment after the first)
 */
template<typename T> inline bool IsMergable(const T& first, const T& second) {
    bool is_consecutive = false;
    bool is_same_style  = false;
    bool is_empty       = false;

    if constexpr(is_segment_ptr<T>) {
        is_consecutive = second->_start == first->_start + first->_str.length();
        is_same_style  = second->_style == first->_style;
        is_empty       = second->_str.isEmpty();
    } else if constexpr(is_segment<T>) {
        is_consecutive = second._start == first._start + first._str.length();
        is_same_style  = first._style == second._style;
        is_empty       = second._str.isEmpty();
    }

    return is_consecutive && is_same_style || is_empty;
}

/**
 * @brief Checks if the index is within the bounds of the segment
 */
template<typename T> inline bool IsInSegment(std::size_t index, const T& segment) {
    bool is_after_start = false;
    bool is_before_end  = false;

    if constexpr(is_segment_ptr<T>) {
        is_after_start = segment->_start <= index;
        is_before_end  = segment->_start + segment->_str.length() > index;
    } else if constexpr(is_segment<T>) {
        is_after_start = segment._start <= index;
        is_before_end  = segment._start + segment._str.length() > index;
    }

    return is_after_start && is_before_end;
}

/**
 * @brief Helper that creates a unique_ptr to a Segment with the parameters
 */
std::unique_ptr<StyledSegmentString::Segment> CreateSegment(const icu::UnicodeString& str, const Style& style, uint64_t start) {
    return std::move(std::make_unique<StyledSegmentString::Segment>(str, style, start));
}

void StyledSegmentString::Append(const icu::UnicodeString& str, const Style& style) {
    if(_segments.empty()) return;

    if(IsMergable(_segments.back(), str, style, Len())) {
        _segments.back().get()->_str.append(str);
    } else {
        _segments.push_back(CreateSegment(str, style, Len()));
    }
}

bool StyledSegmentString::IsInSegment(std::size_t index) const {
    if(_segments.empty()) return false;

    const Segment& segment = *_segments[GetSegmentIndex(index)];

    return LibTesix::IsInSegment(index, _segments[GetSegmentIndex(index)]);
}

void StyledSegmentString::Merge(std::size_t first_index, std::size_t second_index) {
    Segment& first  = *_segments[first_index];
    Segment& second = *_segments[second_index];

    first._str.append(second._str);

    _segments.erase(_segments.begin() + second_index);
}

void StyledSegmentString::InsertSegment(const icu::UnicodeString& str, const Style& style, uint64_t start, std::size_t index) {
    if(_segments.empty()) {
        _segments.push_back(CreateSegment(str, style, start));
        return;
    }

    std::size_t prev_index = index > 0 ? index - 1 : 0;
    std::size_t next_index = index != 0 ? std::min(index + 1, _segments.size() - 1) : 0;

    Segment& prev = *_segments[prev_index];
    Segment& next = *_segments[next_index];

    if(IsMergable(prev, str, style, start)) {
        prev._str.append(str);
    } else if(IsMergable(str, style, start, next)) {
        next._str.insert(0, str);
    } else {
        _segments.insert(_segments.begin() + index, CreateSegment(str, style, start));
    }
}

#ifdef NDEBUG
void StyledSegmentString::PrintDebug() const {
    for(const auto& segment : _segments) {
        std::string utf_8("");
        segment->_str.toUTF8String(utf_8);

        printf("%s|%u\n", utf_8.c_str(), segment->_start);
    }
}
#endif

void StyledSegmentString::Erase(std::size_t start, std::size_t end) {
    if(_segments.empty()) return;
    if(start >= Len()) return;

    uint64_t start_index = GetSegmentIndex(start);
    uint64_t end_index   = GetSegmentIndex(end);

    if(start_index == end_index) {
        Segment& segment = *_segments[start_index];

        uint64_t erase_start = start - segment._start;
        uint64_t erase_len   = (end - segment._start + 1) - erase_start;

        icu::UnicodeString split_str(segment._str, erase_start + erase_len);

        if(split_str.isEmpty()) {
            _segments.insert(_segments.begin() + start_index + 1, CreateSegment(split_str, segment._style, end));
        }

        segment._str.remove(erase_start);
    } else {
        Segment& start_segment = *_segments[start_index];
        Segment& end_segment   = *_segments[end_index];

        _segments.erase(_segments.begin() + start_index + 1, _segments.begin() + end_index);

        if(IsInSegment(start)) {
            start_segment._str.remove(start - start_segment._start);
        }

        end_segment._str.remove(0, end - end_segment._start + 1);
        end_segment._start = end;
    }
}

StyledSegmentString::StyledSegmentString(const StyledSegmentString& str) {
    for(const auto& segment : str._segments) {
        _segments.push_back(std::make_unique<Segment>(*segment));
    }
}

template<typename T> inline void ReplaceSegmentSubStr(const T& segment, std::size_t start, const icu::UnicodeString& str) {
    if constexpr(is_segment_ptr<T>) {
        std::size_t erase_start = start - segment->_start;
        uint64_t    erase_len   = str.length();

        segment->_str.remove(erase_start, erase_len);
        segment->_str.insert(erase_start, str);
    } else if constexpr(is_segment<T>) {
        std::size_t erase_start = start - segment._start;
        uint64_t    erase_len   = str.length();

        segment._str.remove(erase_start, erase_len);
        segment._str.insert(erase_start, str);
    }
}

void StyledSegmentString::Add(const icu::UnicodeString& str, const Style& style, std::size_t index) {
    if(_segments.empty()) {
        _segments.push_back(CreateSegment(str, style, index));
        return;
    }

    std::size_t start_segment_index = GetSegmentIndex(index);
    std::size_t end_segment_index   = GetSegmentIndex(index + str.length());

    if(start_segment_index == end_segment_index) {
        ReplaceSegmentSubStr(_segments[start_segment_index], index, str);
    } else {
        Erase(index, index + str.length() - 1);

        InsertSegment(str, style, index, start_segment_index);
    }
}

void StyledSegmentString::Insert(const icu::UnicodeString& str, const Style& style, std::size_t index) {
    if(index >= Len()) {
        InsertSegment(str, style, index, _segments.size());
        return;
    }

    std::size_t segment_index = GetSegmentIndex(index);
    bool        is_in_segment = IsInSegment(index);

    if(IsInSegment(index)) {
        Segment& segment = *_segments[segment_index];

        icu::UnicodeString split_str(segment._str, index - segment._start);

        segment._str.remove(index - segment._start);

        _segments.insert(_segments.begin() + segment_index + 1, CreateSegment(split_str, segment._style, index));
    }

    InsertSegment(str, style, index, segment_index + is_in_segment);

    for(uint64_t i = segment_index + 1; i < _segments.size(); i++) {
        _segments[i]->_start += str.length();
    }
}

uint64_t StyledSegmentString::Len() const {
    if(_segments.empty()) return 0;

    return _segments.back()->_start + _segments.back()->_str.length();
}

uint64_t StyledSegmentString::GetSegmentIndex(std::size_t index) const {
    if(_segments.empty()) return 0;

    if(index >= _segments.back()->_start) return _segments.size() - 1;
    if(index <= _segments.front()->_start) return 0;

    uint64_t range_start = 0;
    uint64_t range_end   = _segments.size() - 1;

    uint64_t cur = range_end >> 1;

    while(true) {
        bool is_after_cur   = index >= _segments[cur].get()->_start;
        bool is_before_next = _segments[std::min(_segments.size() - 1, cur + 1)].get()->_start > index;

        if(is_after_cur && is_before_next) return cur;

        if(_segments[cur].get()->_start > index) {
            range_end = cur;
        } else if(_segments[cur].get()->_start < index) {
            range_start = cur < _segments.size() - 1 ? cur + 1 : cur;
        }

        cur = std::midpoint(range_start, range_end);
    }
}

StyledSegmentString::Segment::Segment(const icu::UnicodeString& str, const Style& style, uint64_t start) : _style(style) {
    _str   = str;
    _start = start;
}

StyledSegmentString::Segment::Segment(const Segment&& seg) : _style(seg._style) {
    _str   = seg._str;
    _start = seg._start;
}

StyledSegmentString::Segment::Segment(const Segment& seg) : _style(seg._style) {
    _str   = seg._str;
    _start = seg._start;
}

std::unique_ptr<StyledSegmentString::Segment> StyledSegmentString::Segment::Clone() const {
    return std::make_unique<Segment>(*this);
}

} // namespace LibTesix