#pragma once

#include <tinyutf8/tinyutf8.h>

#include <cinttypes>
#include <memory>
#include <vector>
#include <numeric>

#include "Helpers/Generic.hpp"
#include "Style.hpp"

namespace LibTesix {
/**
 * @brief Represents a styled region with its contents
 */
struct Segment {
  public:
    Segment(const tiny_utf8::string& str, const Style& style, uint64_t start) : _style(style) {
        _str   = str;
        _start = start;
    }

    uint64_t          _start;
    const Style&      _style;
    tiny_utf8::string _str;
};

/**
 * @brief Stores a character with a style
 */
struct StyledChar {
  public:
    StyledChar(char32_t character, const Style& style) : _style(style) {
        _char = character;
    }

    char32_t     _char;
    const Style& _style;

    bool operator==(const StyledChar& other) const {
        return _char == other._char && _style == other._style;
    }
};

typedef std::unique_ptr<Segment> _SegmentPtr;

} // namespace LibTesix

#include "Helpers/StyledString.hpp"

namespace LibTesix {
/**
 * @brief The base of StyledString. This contains functions that can be used in both StyledString specializations. This
 * is basically an array of segments.
 */
struct StyledStringBase {
  protected:
    StyledStringBase() = default;

    StyledStringBase(const StyledStringBase& that) {
        for(auto& seg : that._segments) {
            _segments.push_back(std::make_unique<Segment>(*seg));
        }
    }

    /**
     * @brief Gets the lenght of the string
     * @returns The lenght (The end of the last segment plus one)
     */
    uint64_t Len() {
        if(_segments.empty()) return 0;

        return GetSegmentEnd(_segments.back()) + 1;
    }

    /**
     * @brief Appends a segment to the string
     * @param str The contents of the new segment
     * @param style The style of the segment
     */
    void Append(const tiny_utf8::string& str, const Style& style) {
        AppendSegment(CreateSegment(str, style, Len()));
    }

#ifdef NDEBUG
    /**
     * @brief Prints debug info of the string to stdout
     */
    void PrintDebug() {
        for(auto& seg : _segments) {
            printf("%s|%u\n", seg->_str.c_str(), seg->_start);
        }
    }
#endif

    std::vector<_SegmentPtr> _segments;

    /**
     * @brief Finds the index of the segment owning the given index with binary search. A segment owns a index if it is
     * after its start and before the next segments, the first segment owns everything prior to it
     * @param index The index to search for.
     * @returns The index of the segment owning the given index.
     */
    std::size_t GetSegmentIndex(std::size_t index) const {
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

    /**
     * @brief Appends a segment to the string and merges if possible.
     * Doesnt check if the segment would be illegal
     * @param segment The segment to append
     * @returns Whether or not the inserted segment was inserted by merging
     */
    bool AppendSegment(_SegmentPtr segment) {
        _SegmentPtr& last = _segments.back();

        if(!_segments.empty() && IsMergable(last, segment)) {
            last->_str.append(segment->_str);
            return true;
        } else {
            _segments.push_back(std::move(segment));
            return false;
        }
    }

    /**
     * @brief Inserts a segment to the string automatically merges if suitable.
     * Doesnt chack if the new segment is illegal.
     * @param seg The new segment
     * @param index The index(s) where to insert
     * @returns Whether or not the inserted segment was inserted by merging
     */
    bool InsertSegment(_SegmentPtr segment, std::size_t index) {
#ifndef LIBTESIX_NO_SAFETY
        if(index > _segments.size()) {
            throw std::range_error("Index is out of bounds! << StyledStringBase::InsertSegment()");
        }
#endif

        if(index == _segments.size()) return AppendSegment(std::move(segment));

        Segment& prev = *GetPrev(index, _segments);
        Segment& next = *_segments[index];

        if(IsMergable(prev, *segment)) {
            prev._str.append(segment->_str);
            return true;
        } else if(IsMergable(*segment, next)) {
            next._str.insert(0, segment->_str);
            return true;
        }

        _segments.insert(_segments.begin() + index, std::move(segment));
        return false;
    }

    /**
     * @brief Moves a segment to the destination and merges if possible.
     * Doesnt check if the new position is illegal.
     * @param seg_index The index of the segmnent
     * @param dest The destination
     * @returns Whether or not the inserted segment was moved by merging
     */
    bool MoveSegment(std::size_t seg_index, uint64_t dest) {
#ifndef LIBTESIX_NO_SAFETY
        if(seg_index >= _segments.size()) {
            throw std::range_error("Index is out of bounds! << StyledStringBase::MoveSegment()");
        }
#endif

        Segment& seg = *_segments[seg_index];

        std::size_t new_segment_index = GetSegmentIndex(dest);

        Segment& prev = *GetPrev(new_segment_index, _segments);
        Segment& next = *GetNext(new_segment_index, _segments);

        seg._start = dest;

        if(IsMergable(prev, seg._str, seg._style, dest)) {
            prev._str.append(seg._str);
            _segments.erase(_segments.begin() + seg_index);
            return true;
        } else if(IsMergable(seg._str, seg._style, dest, next)) {
            next._str.insert(0, seg._str);
            _segments.erase(_segments.begin() + seg_index);
            return true;
        }

        return false;
    }

    /**
     * @brief Inserts a segment at the index(g), moves following segments back if there is no space
     * @param str The contents of the new segment
     * @param style The style of the segment
     * @param index The index(g) where the segment should be inserted
     */
    void Insert(const tiny_utf8::string& str, const Style& style, std::size_t index) {
        if(index >= Len()) {
            InsertSegment(CreateSegment(str, style, index), _segments.size());
            return;
        }

        std::size_t seg_index = GetSegmentIndex(index);

        for(uint64_t i = seg_index + 1; i < _segments.size(); i++) {
            _segments[i]->_start += str.length();
        }

        Segment& seg = *_segments[seg_index];

        bool in_segment   = IsInSegment(index, seg);
        bool can_insert   = in_segment && style == seg._style;
        bool should_split = index != seg._start;

        if(can_insert) {
            seg._str.insert(index - seg._start, str);
        } else if(should_split) {
            _SegmentPtr split = SplitSegment(seg, index - seg._start, 0);
            split->_start += str.length();
            InsertSegment(std::move(split), seg_index + 1);

            InsertSegment(CreateSegment(str, style, index), seg_index + 1);
        } else {
            InsertSegment(CreateSegment(str, style, index), seg_index + 1);
        }
    }

    /**
     * @brief Erases the string in the given range
     * @param start The starting index(g) of the range
     * @param end The ending index(g) of the range
     */
    void Erase(std::size_t start = 0, uint64_t len = UINT64_MAX) {
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
                SegmentErase(end_seg, 0, end - end_seg._start + 1);
            }
        }
    }
};

template<bool contiguous> class StyledString {};

/**
 * @brief A non contiguous SegmentString. Segments cant overlap so this {{"foo", 0},{"bar", 1}} would be illegal since
 * foo occupies the indecies between 0 and 2, because it is 3 characters long so 0:f 1:o 2:o. {{"foo", 0}, {"bar", 6}}
 * would be legal, because there is only 1 Segment that "owns" each character.
 */
template<> class StyledString<false> : private StyledStringBase {
    typedef StyledString<false> _type;

  public:
    StyledString() = default;

    StyledString(const _type& that) : StyledStringBase(that) {
    }

    using StyledStringBase::Len;

    using StyledStringBase::Append;

    using StyledStringBase::Insert;

#ifdef NDEBUG
    using StyledStringBase::PrintDebug;
#endif

    using StyledStringBase::Erase;
    /**
     * @brief Adds a segment at the index(g), overwrites other segments
     * @param str The contents of the new segment
     * @param style The style of the segment
     * @param index The index(g) where the segment should be inserted
     */
    void Add(const tiny_utf8::string& str, const Style& style, std::size_t index) {
        if(str.empty()) return;

        if(_segments.empty()) {
            _segments.push_back(CreateSegment(str, style, index));
            return;
        }

        std::size_t start = index;
        std::size_t end   = index + str.length() - 1;

        std::size_t seg_index = GetSegmentIndex(index);

        Segment& seg = *_segments[seg_index];

        bool within_start_segment = IsRangeInSegment(start, end, seg);
        bool is_same_style        = style == seg._style;

        if(within_start_segment && is_same_style) {
            SegmentReplaceInplace(seg, str, index - seg._start);
        } else {
            Erase(start, str.length());

            bool should_insert_in_front = !(index <= seg._start);

            InsertSegment(CreateSegment(str, style, index), seg_index + should_insert_in_front);
        }
    }

    struct Reference {
        friend _type;

      public:
        void operator=(const StyledChar& character) {
            _src.Set(character, _index);
        }

      private:
        Reference(_type& src, std::size_t index) : _src(src) {
            _index = index;
        }

        _type&      _src;
        std::size_t _index;
    };

    /**
     * @brief Sets the character at the index
     * @param character The value to set to
     * @param index The index(g)
     */
    void Set(const StyledChar& character, std::size_t index) {
        std::size_t seg_index = GetSegmentIndex(index);
        Segment&    segment   = *_segments[seg_index];

        bool is_in_segment = IsInSegment(index, segment);
        bool is_same_style = segment._style == character._style;

        if(is_in_segment && is_same_style) {
            segment._str[index - segment._start] = character._char;
            return;
        }

        if(is_in_segment) {
            Erase(index, 1);
        }

        bool insert_in_front = index > segment._start;
        InsertSegment(CreateSegment(character, index), seg_index + insert_in_front);
    }

    Reference operator[](std::size_t index) {
        return Reference(*this, index);
    }

    /**
     * @brief Gets the character at the index
     * @param index The index(g)
     */
    StyledChar CharAt(std::size_t index, const Style& fallback) {
        const Segment& seg = *_segments[GetSegmentIndex(index)];

        if(!IsInSegment(index, seg)) {
            return StyledChar(0, fallback);
        } else {
            return StyledChar(seg._str[index - seg._start], seg._style);
        }
    }
};

/**
 * @brief A contiguous StyledString. In addition to the rule set for non contiguous StyledString segments have to be
 * contiguous. Segments need to be one after another for example like this {{"foo", 0}, {"bar", 3}}. Foo starts and 0
 * and ends at 2 since foo is 3 characters long and bar is exactly one more than that. This way holes dont appear
 * withing the string.
 */
template<> class StyledString<true> : private StyledStringBase {
    typedef StyledString<true> _type;

  public:
    StyledString() = default;

    StyledString(const _type& that) : StyledStringBase(that) {
    }

    using StyledStringBase::Len;

    using StyledStringBase::Append;

    using StyledStringBase::Insert;

#ifdef NDEBUG
    using StyledStringBase::PrintDebug;
#endif

    /**
     * @brief Erases the string in the given range
     * @param start The starting index(g) of the range
     * @param end The ending index(g) of the range
     */
    void Erase(std::size_t start = 0, uint64_t len = UINT64_MAX) {
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
            } else {
                InsertSegment(SplitSegment(start_seg, start - start_seg._start, end - start + 1), start_index + 1);
            }
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
                SegmentErase(end_seg, 0, end - end_seg._start + 1);
            }
        }

        for(std::size_t i = start_index; i < _segments.size();) {
            if(!MoveSegment(i, _segments[i]->_start - len)) i++;
        }
    }

    struct Reference {
        friend _type;

      public:
        void operator=(const StyledChar& character) {
            _src.Set(character, _index);
        }

      private:
        Reference(_type& src, std::size_t index) : _src(src) {
            _index = index;
        }

        _type&      _src;
        std::size_t _index;
    };

    /**
     * @brief Sets the character at the index
     * @param character The value to set to
     * @param index The index(g)
     */
    void Set(const StyledChar& character, std::size_t index) {
#ifndef LIBTESIX_NO_SAFETY
        if(index >= Len()) {
            throw std::range_error("Reference is out of bounds! << StyledString<true>::Set()");
        }
#endif

        std::size_t seg_index = GetSegmentIndex(index);
        Segment&    seg       = *_segments[seg_index];

        bool is_same_style = seg._style == character._style;

        if(is_same_style) {
            seg._str[index - seg._start] = character._char;
            return;
        }

        StyledStringBase::Erase(index, 1);

        bool insert_in_front = index > seg._start;
        InsertSegment(CreateSegment(character, index), seg_index + insert_in_front);
    }

    Reference operator[](std::size_t index) {
#ifndef LIBTESIX_NO_SAFETY
        if(index >= Len()) {
            throw std::range_error("Index is out of bounds! << StyledString<true>::operator[]()");
        }
#endif

        return Reference(*this, index);
    }

    /**
     * @brief Gets the character at the index
     * @param index The index(g)
     */
    StyledChar CharAt(std::size_t index) {
#ifndef LIBTESIX_NO_SAFETY
        if(index >= Len()) {
            throw std::range_error("Index is out of bounds! << StyledString<true>::CharAt()");
        }
#endif

        const Segment& seg = *_segments[GetSegmentIndex(index)];

        return StyledChar(seg._str[index - seg._start], seg._style);
    }
};

} // namespace LibTesix