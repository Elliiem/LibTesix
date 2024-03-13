#include "StyledSegmentString.hpp"

#include "Helpers/Generic.hpp"
#include "Helpers/StyledSegmentString.hpp"

namespace LibTesix {

void StyledSegmentString::Append(const tiny_utf8::string& str, const Style& style) {
    InsertSegment(str, style, Len(), _segments.size());
}

bool StyledSegmentString::InsertSegment(
    const tiny_utf8::string& str, const Style& style, uint64_t start, std::size_t index) {
    return InsertSegment(CreateSegment(str, style, start), index);
}

bool StyledSegmentString::InsertSegment(std::unique_ptr<Segment> seg, std::size_t index) {
    if(_segments.empty()) {
        _segments.push_back(std::move(seg));
        return false;
    }

    Segment& prev = *GetPrev(index, _segments);
    Segment& next = *GetNext(index, _segments);

    if(IsMergable(prev, *seg)) {
        prev._str.append(seg->_str);
        return true;
    } else if(IsMergable(*seg, next)) {
        next._str.insert(0, seg->_str);
        next._start = seg->_start;
        return true;
    } else {
        _segments.insert(_segments.begin() + index, std::move(seg));
        return false;
    }
}

bool StyledSegmentString::MoveSegment(std::size_t segment_index, uint64_t dest) {
#ifndef LIBTESIX_NO_SAFETY
    if(segment_index >= _segments.size()) {
        throw std::range_error("Index is out of bounds! << StyledSegmentString::MoveSegment()");
    }
#endif

    Segment& seg = *_segments[segment_index];

    std::size_t new_segment_index = GetSegmentIndex(dest);

    Segment& prev = *GetPrev(new_segment_index, _segments);
    Segment& next = *GetNext(new_segment_index, _segments);

    if(IsMergable(prev, seg._str, seg._style, dest)) {
        prev._str.append(seg._str);
    } else if(IsMergable(seg._str, seg._style, dest, next)) {
        next._str.insert(0, seg._str);
    } else {
        seg._start = dest;
        return false;
    }

    _segments.erase(_segments.begin() + segment_index);
    return true;
}

#ifdef NDEBUG
void StyledSegmentString::PrintDebug() const {
    for(const auto& segment : _segments) {
        printf("%s|%u\n", segment->_str.c_str(), segment->_start);
    }
}
#endif

void StyledSegmentString::Erase(std::size_t start, std::size_t len) {
    if(_segments.empty()) return;
    if(start >= Len()) return;

    std::size_t end = start + len - 1;

    uint64_t start_index = GetSegmentIndex(start);
    uint64_t end_index   = GetSegmentIndex(end);

    Segment& start_seg = *_segments[start_index];
    Segment& end_seg   = *_segments[end_index];

    bool is_in_single_segment = LibTesix::IsRangeInSegment(start, end, start_seg);

    if(is_in_single_segment) {
        bool at_start = start <= start_seg._start;
        bool at_end   = end >= GetSegmentEnd(start_seg);

        bool should_erase_segment = at_start && at_end;

        if(should_erase_segment) {
            _segments.erase(_segments.begin() + start_index);
            return;
        }

        InsertSegment(SplitSegment(start_seg, start - start_seg._start, end - start + 1), start_index + 1);
    } else {
        bool should_erase_start = start <= start_seg._start;
        bool should_erase_end   = end >= GetSegmentEnd(end_seg);

        auto erase_start_iter = _segments.begin() + start_index + !should_erase_start;
        auto erase_end_iter   = _segments.begin() + end_index + should_erase_end;

        _segments.erase(erase_start_iter, erase_end_iter);

        if(!should_erase_start) {
            SegmentErase(start_seg, start - start_seg._start);
        }

        if(!should_erase_end) {
            SegmentErase(end_seg, 0, end - end_seg._start);
        }
    }
}

StyledSegmentString::StyledSegmentString(const StyledSegmentString& str) {
    for(const auto& segment : str._segments) {
        _segments.push_back(std::make_unique<Segment>(*segment));
    }
}

void StyledSegmentString::Add(const tiny_utf8::string& str, const Style& style, std::size_t index) {
    if(str.empty()) return;

    if(_segments.empty()) {
        _segments.push_back(CreateSegment(str, style, index));
        return;
    }

    std::size_t start = index;
    std::size_t end   = index + str.length() - 1;

    std::size_t segment_index = GetSegmentIndex(index);

    Segment& seg = *_segments[segment_index];

    bool within_start_segment = IsRangeInSegment(start, end, seg);
    bool is_same_style        = style == seg._style;

    if(within_start_segment && is_same_style) {
        SegmentReplaceInplace(seg, str, index - seg._start);
    } else {
        Erase(start, str.length());

        bool should_insert_in_front = !(index <= seg._start);

        InsertSegment(str, style, index, segment_index + should_insert_in_front);
    }
}

void StyledSegmentString::Insert(const tiny_utf8::string& str, const Style& style, std::size_t index) {
    if(index >= Len()) {
        InsertSegment(str, style, index, _segments.size());
        return;
    }

    std::size_t segment_index = GetSegmentIndex(index);

    for(uint64_t i = segment_index + 1; i < _segments.size(); i++) {
        _segments[i]->_start += str.length();
    }

    Segment& seg = *_segments[segment_index];

    bool in_segment   = IsInSegment(index, seg);
    bool can_insert   = in_segment && style == seg._style;
    bool should_split = in_segment && index != seg._start;

    if(can_insert) {
        seg._str.insert(index - seg._start, str);
    } else if(should_split) {
        _SegmentPtr split = SplitSegment(seg, index - seg._start, 0);
        split->_start += str.length();
        InsertSegment(std::move(split), segment_index + 1);

        InsertSegment(str, style, index, segment_index + 1);
    } else {
        InsertSegment(str, style, index, segment_index + 1);
    }
}

uint64_t StyledSegmentString::Len() const {
    if(_segments.empty()) return 0;

    return GetSegmentEnd(_segments.back()) + 1;
}

void StyledSegmentString::Set(const StyledChar& character, std::size_t index) {
    if(character._char == 0) {
        return;
    }

    std::size_t segment_index = GetSegmentIndex(index);

    Segment& seg = *_segments[segment_index];

    bool is_same_style = seg._style == character._style;
    bool is_in_segment = IsInSegment(index, seg);

    if(is_in_segment && is_same_style) {
        seg._str[index - seg._start];
    } else {
        Erase(index, 1);
    }

    bool should_insert_in_front = !(index <= seg._start);

    tiny_utf8::string char_str(character._char);
    InsertSegment(char_str, character._style, index, segment_index + should_insert_in_front);
}

std::size_t StyledSegmentString::GetSegmentIndex(std::size_t index) const {
    if(_segments.empty()) return 0;

    if(index >= _segments.back()->_start) return MaxIndex(_segments);
    if(index <= _segments.front()->_start) return 0;

    uint64_t range_start = 0;
    uint64_t range_end   = MaxIndex(_segments);

    uint64_t cur = range_end >> 1;

    while(true) {
        const Segment& cur_seg  = *_segments[cur];
        const Segment& next_seg = *GetNext(cur, _segments);

        bool is_after_cur   = index >= cur_seg._start;
        bool is_before_next = next_seg._start > index;

        if(is_after_cur && is_before_next) return cur;

        if(cur_seg._start > index) {
            range_end = DecrBounded(cur, 0);
        } else if(cur_seg._start < index) {
            range_start = IncrBounded(cur, MaxIndex(_segments));
        }

        cur = std::midpoint(range_start, range_end);
    }
}

StyledSegmentString::Segment::Segment(const tiny_utf8::string& str, const Style& style, uint64_t start) :
    _style(style) {
    _str   = str;
    _start = start;
}

StyledSegmentString::Reference::Reference(StyledSegmentString& src, std::size_t index) : _src(src) {
    _index = index;
}

StyledSegmentString::Reference StyledSegmentString::operator[](std::size_t index) {
    return Reference(*this, index);
}

StyledSegmentString::StyledChar::StyledChar(char32_t character, const Style& style) : _style(style) {
    _char = character;
}

void StyledSegmentString::Reference::operator=(const StyledChar& character) {
    _src.Set(character, _index);
}

StyledSegmentString::Reference::operator StyledChar() const {
    const Segment& seg = *_src._segments[_src.GetSegmentIndex(_index)];

#ifndef LIBTESIX_NO_SAFETY
    if(!IsInSegment(_index, seg)) {
        throw std::runtime_error("Reference points to a void character! << Reference::operator StyledChar()");
    }
#endif

    return StyledChar(seg._str[_index - seg._start], seg._style);
}

void StyledSegmentString::Reference::operator=(const Reference& character) {
    _src.Set(character, _index);
}

bool StyledSegmentString::Reference::IsVoid() {
    const Segment& seg = *_src._segments[_src.GetSegmentIndex(_index)];

    return !IsInSegment(_index, seg);
}

} // namespace LibTesix