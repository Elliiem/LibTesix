#include "StyledSegmentString.hpp"

#include "Helpers/Generic.hpp"
#include "Helpers/StyledSegmentString.hpp"

namespace LibTesix {

void StyledSegmentString::Append(const tiny_utf8::string& str, const Style& style) {
    InsertSegment(str, style, Len(), _segments.size());
}

void StyledSegmentString::InsertSegment(
    const tiny_utf8::string& str, const Style& style, uint64_t start, std::size_t index) {
    InsertSegment(CreateSegment(str, style, start), index);
}

void StyledSegmentString::InsertSegment(std::unique_ptr<Segment> seg, std::size_t index) {
    if(_segments.empty()) {
        _segments.push_back(std::move(seg));
        return;
    }

    Segment& prev = *GetPrev(index, _segments);
    Segment& next = *GetNext(index, _segments);

    if(IsMergable(prev, *seg)) {
        prev._str.append(seg->_str);
    } else if(IsMergable(*seg, next)) {
        next._str.insert(0, seg->_str);
        next._start = seg->_start;
    } else {
        _segments.insert(_segments.begin() + index, std::move(seg));
    }
}

#ifdef NDEBUG
void StyledSegmentString::PrintDebug() const {
    for(const auto& segment : _segments) {
        printf("%s|%u\n", segment->_str.c_str(), segment->_start);
    }
}
#endif

void StyledSegmentString::Erase(std::size_t start, std::size_t end) {
    if(_segments.empty()) return;
    if(start >= Len()) return;

    uint64_t start_index = GetSegmentIndex(start);
    uint64_t end_index   = GetSegmentIndex(end);

    Segment& start_seg = *_segments[start_index];
    Segment& end_seg   = *_segments[end_index];

    if(LibTesix::IsRangeInSegment(start, end, start_seg)) {
        _SegmentPtr split = SplitSegment(start_seg, start - start_seg._start, end - start + 1);

        InsertSegment(std::move(split), start_index + 1);
    } else {
        bool erase_start = start <= start_seg._start;
        bool erase_end   = end >= end_seg._start + end_seg._str.length() - 1;

        auto erase_start_iter = _segments.begin() + start_index + !erase_start;
        auto erase_end_iter   = _segments.begin() + end_index + erase_end;

        _segments.erase(erase_start_iter, erase_end_iter);

        if(!erase_start) {
            SegmentErase(start_seg, start - start_seg._start, SIZE_MAX);
        }

        if(!erase_end) {
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
    if(_segments.empty()) {
        _segments.push_back(CreateSegment(str, style, index));
        return;
    }

    std::size_t start = index;
    std::size_t end   = index;

    std::size_t start_index = GetSegmentIndex(index);

    Segment& seg = *_segments[start_index];
    if(IsRangeInSegment(start, end, seg) && style == seg._style) {
        SegmentReplaceInplace(seg, str, index - seg._start);
    } else {
        Erase(start, end);

        InsertSegment(str, style, index, start_index);
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

    if(IsInSegment(index, seg) && index != seg._start) {
        _SegmentPtr split = SplitSegment(seg, index - seg._start, 0);
        split->_start += str.length();

        InsertSegment(str, style, index, segment_index + 1);
        InsertSegment(std::move(split), segment_index + 1);
    } else {
        InsertSegment(str, style, index, segment_index + 1);
    }
}

uint64_t StyledSegmentString::Len() const {
    if(_segments.empty()) return 0;

    return _segments.back()->_start + _segments.back()->_str.length();
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

} // namespace LibTesix