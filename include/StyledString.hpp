#pragma once

#include <tinyutf8/tinyutf8.h>

#include <cinttypes>
#include <memory>
#include <vector>
#include <numeric>

#include "Style.hpp"
#include "Helpers/Generic.hpp"

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
        for(auto& segment : that._segments) {
            _segments.push_back(std::make_unique<Segment>(*segment));
        }
    }

    std::vector<_SegmentPtr> _segments;

    /**
     * @brief Gets the lenght of the string
     * @returns The lenght (The end of the last segment plus one)
     */
    uint64_t Len() const {
        if(_segments.empty()) return 0;

        return GetSegmentEnd(_segments.back()) + 1;
    }

#ifdef NDEBUG
    /**
     * @brief Prints debug info of the string to stdout
     */
    void PrintDebug() const {
        for(auto& segment : _segments) {
            printf("%s|%u\n", segment->_str.c_str(), segment->_start);
        }
    }
#endif

    /**
     * @brief Gets the segments of this string
     * @returns A const reference to the segments
     */
    const std::vector<_SegmentPtr>& GetSegments() {
        return _segments;
    }

    /**
     * @brief Finds the index of the segment owning the given index with binary search. A segment owns a index if it is
     * after its start and before the next segments, the first segment owns everything prior to it
     * @param index The index to search for.
     * @returns The index of the segment owning the given index.
     */
    uint64_t GetSegmentIndex(uint64_t index) const {
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
        if(!_segments.empty() && IsMergable(_segments.back(), segment)) {
            _segments.back()->_str.append(segment->_str);
            return true;
        }

        _segments.push_back(std::move(segment));
        return false;
    }

    /**
     * @brief Inserts a segment to the string automatically merges if suitable.
     * Doesnt chack if the new segment is illegal.
     * @param segment The new segment
     * @param index The index(s) where to insert
     * @returns Whether or not the inserted segment was inserted by merging
     */
    bool InsertSegment(_SegmentPtr segment, uint64_t index) {
        if(index > _segments.size()) {
            throw std::range_error("Index is out of bounds! << StyledStringBase::InsertSegment()");
        }

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
     * @param segment_index The index of the segmnent
     * @param dest The destination
     * @returns Whether or not the inserted segment was moved by merging
     */
    bool MoveSegment(uint64_t segment_index, uint64_t dest) {
        if(segment_index >= _segments.size()) {
            throw std::range_error("Index is out of bounds! << StyledStringBase::MoveSegment()");
        }

        Segment& segment = *_segments[segment_index];

        segment._start = dest;

        uint64_t dest_segment_index = GetSegmentIndex(dest);

        Segment& prev = *GetPrev(dest_segment_index, _segments);
        Segment& next = *GetNext(dest_segment_index, _segments);

        if(IsMergable(prev, segment._str, segment._style, dest)) {
            prev._str.append(segment._str);
            _segments.erase(_segments.begin() + segment_index);
            return true;
        } else if(IsMergable(segment._str, segment._style, dest, next)) {
            next._str.insert(0, segment._str);
            _segments.erase(_segments.begin() + segment_index);
            return true;
        }

        return false;
    }

    /**
     * @brief Pushes the segments in the range [start, end] by the ammount
     * @param start The index of the first segment that will get moved
     * @param ammount The ammount to move the segment, positive to move the segments back,
     * negative to push them forward
     * @param end The index of the last segment
     */
    void PushSegments(uint64_t start, int64_t ammount, uint64_t end = UINT64_MAX) {
        if(_segments.empty()) {
            // TODO Throw ?
            return;
        }

        end = std::min(_segments.size() - 1, end);

        for(uint64_t i = end; i >= start; i--) {
            MoveSegment(i, _segments[i]->_start + ammount);
            if(i == 0) break;
        }
    }
};

enum Contiguous { CONT, NCONT };
template<Contiguous> class StyledString {};

/**
 * @brief A non contiguous SegmentString. Segments cant overlap so this {{"foo", 0},{"bar", 1}} would be illegal
 * since foo occupies the indecies between 0 and 2, because it is 3 characters long so 0:f 1:o 2:o. {{"foo", 0},
 * {"bar", 6}} would be legal, because there is only 1 Segment that "owns" each character.
 */
template<> class StyledString<NCONT> : private StyledStringBase {
    typedef StyledString<NCONT> _type;

  public:
    StyledString() = default;

    StyledString(const _type& that) : StyledStringBase(that) {
    }

    using StyledStringBase::Len;

#ifdef NDEBUG
    using StyledStringBase::PrintDebug;
#endif

    using StyledStringBase::GetSegments;

    /**
     * @brief Appends a segment to the string
     * @param str The contents of the new segment
     * @param style The style of the segment
     */
    void Append(const tiny_utf8::string& str, const Style& style) {
        AppendSegment(CreateSegment(str, style, Len()));
    }

    /**
     * @brief Erases the string in the given range
     * @param start The starting index(g) of the range
     * @param end The ending index(g) of the range
     */
    void Erase(uint64_t start = 0, uint64_t len = UINT64_MAX) {
        if(start >= Len()) return;

        uint64_t end = start + len - 1;

        uint64_t start_index   = GetSegmentIndex(start);
        Segment& start_segment = *_segments[start_index];
        uint64_t end_index     = GetSegmentIndex(end);
        Segment& end_segment   = *_segments[end_index];

        bool within_one_segment = LibTesix::IsRangeInSegment(start, end, start_segment);

        if(within_one_segment && !IsInSegment(start, start_segment)) return;

        if(within_one_segment) {
            bool at_start = start <= start_segment._start;
            bool at_end   = end >= GetSegmentEnd(start_segment);

            bool should_erase_segment = at_start && at_end;

            if(should_erase_segment) {
                _segments.erase(_segments.begin() + start_index);
                return;
            }

            InsertSegment(SplitSegment(start_segment, start - start_segment._start, end - start + 1), start_index + 1);
        } else {
            bool should_erase_start = start <= start_segment._start;
            bool should_erase_end   = end >= GetSegmentEnd(end_segment);

            auto erase_start_iter = _segments.begin() + start_index + !should_erase_start;
            auto erase_end_iter   = _segments.begin() + end_index + should_erase_end;

            _segments.erase(erase_start_iter, erase_end_iter);

            if(!should_erase_start) {
                SegmentErase(start_segment, start - start_segment._start);
            }

            if(!should_erase_end) {
                SegmentErase(end_segment, 0, end - end_segment._start + 1);
            }
        }
    }

    /**
     * @brief Adds a segment at the index(g), overwrites other segments
     * @param str The contents of the new segment
     * @param style The style of the segment
     * @param index The index(g) where the segment should be inserted
     */
    void Add(const tiny_utf8::string& str, const Style& style, uint64_t index) {
        if(str.empty()) return;

        if(_segments.empty()) {
            _segments.push_back(CreateSegment(str, style, index));
            return;
        }

        uint64_t start = index;
        uint64_t end   = GetSegmentEnd(str, index);

        uint64_t segment_index = GetSegmentIndex(index);
        Segment& segment       = *_segments[segment_index];

        bool within_one_segment = IsRangeInSegment(start, end, segment);
        bool same_style         = style == segment._style;

        if(within_one_segment && same_style) {
            SegmentReplaceInplace(segment, str, index - segment._start);
        } else {
            Erase(start, str.length());

            _SegmentPtr new_seg = CreateSegment(str, style, index);

            bool should_insert_in_front = !(index <= segment._start);
            InsertSegment(std::move(new_seg), segment_index + should_insert_in_front);
        }
    }

    /**
     * @brief Inserts a segment at the index(g), moves following segments back if there is no space
     * @param str The contents of the new segment
     * @param style The style of the segment
     * @param index The index(g) where the segment should be inserted
     */
    void Insert(const tiny_utf8::string& str, const Style& style, uint64_t index) {
        if(index >= Len()) {
            InsertSegment(CreateSegment(str, style, index), _segments.size());
            return;
        }

        uint64_t segment_index = GetSegmentIndex(index);
        Segment& segment       = *_segments[segment_index];

        PushSegments(segment_index + 1, str.length());

        bool is_in_segment = IsInSegment(index, segment);
        bool is_same_style = style == segment._style;

        // If the index is inside a segment and the style and the one of the segment are the same we can just insert the
        // string to the string of the segment and return early.
        bool can_insert = is_in_segment && is_same_style;
        if(can_insert) {
            segment._str.insert(index - segment._start, str);
            return;
        }

        _SegmentPtr split = SplitSegment(segment, index - segment._start, 0);
        split->_start += str.length();

        InsertSegment(std::move(split), segment_index + 1);
        InsertSegment(CreateSegment(str, style, index), segment_index + 1);
    }

    struct Reference {
        friend _type;

      public:
        void operator=(const StyledChar& character) {
            _src.Set(character, _index);
        }

      private:
        Reference(_type& src, uint64_t index) : _src(src) {
            _index = index;
        }

        _type&   _src;
        uint64_t _index;
    };

    /**
     * @brief Sets the character at the index
     * @param character The value to set to
     * @param index The index(g)
     */
    void Set(const StyledChar& character, uint64_t index) {
        uint64_t segment_index = GetSegmentIndex(index);
        Segment& segment       = *_segments[segment_index];

        bool in_segment    = IsInSegment(index, segment);
        bool is_same_style = segment._style == character._style;

        // If the index is inside a segment and the character and the segment have the same style we can just set the
        // character at that index of the string, and return early.
        bool can_set = in_segment && is_same_style;
        if(can_set) {
            segment._str[index - segment._start] = character._char;
            return;
        }

        // If the index is inside a segment we need to make space for the character.
        if(in_segment) {
            _SegmentPtr split = SplitSegment(segment, index, 1);

            InsertSegment(std::move(split), segment_index);
        }

        _SegmentPtr new_seg = CreateSegment(character, index);
        InsertSegment(std::move(new_seg), segment_index);
    }

    Reference operator[](uint64_t index) {
        return Reference(*this, index);
    }

    /**
     * @brief Gets the character at the index
     * @param index The index(g)
     */
    StyledChar CharAt(uint64_t index, const Style& fallback) const {
        const Segment& segment = *_segments[GetSegmentIndex(index)];

        if(!IsInSegment(index, segment)) {
            return StyledChar(0, fallback);
        } else {
            return StyledChar(segment._str[index - segment._start], segment._style);
        }
    }
};

/**
 * @brief A contiguous StyledString. In addition to the rule set for non contiguous StyledString segments have to be
 * contiguous. Segments need to be one after another for example like this {{"foo", 0}, {"bar", 3}}. Foo starts and 0
 * and ends at 2 since foo is 3 characters long and bar is exactly one more than that. This way holes dont appear
 * withing the string.
 */
template<> class StyledString<CONT> : private StyledStringBase {
    typedef StyledString<CONT> _type;

  public:
    StyledString() = default;

    StyledString(const _type& that) : StyledStringBase(that) {
    }

    using StyledStringBase::Len;

#ifdef NDEBUG
    using StyledStringBase::PrintDebug;
#endif

    using StyledStringBase::GetSegments;

    /**
     * @brief Appends a segment to the string
     * @param str The contents of the new segment
     * @param style The style of the segment
     */
    void Append(const tiny_utf8::string& str, const Style& style) {
        AppendSegment(CreateSegment(str, style, Len()));
    }

    /**
     * @brief Erases the string in the given range
     * @param start The starting index(g) of the range
     * @param end The ending index(g) of the range
     */
    void Erase(uint64_t start = 0, uint64_t len = UINT64_MAX) {
        if(_segments.empty()) return;
        if(start >= Len()) return;

        uint64_t end = start + len - 1;

        uint64_t start_index   = GetSegmentIndex(start);
        Segment& start_segment = *_segments[start_index];
        uint64_t end_index     = GetSegmentIndex(end);
        Segment& end_segment   = *_segments[end_index];

        bool within_one_segment = LibTesix::IsRangeInSegment(start, end, start_segment);

        if(within_one_segment) {
            bool at_start = start <= start_segment._start;
            bool at_end   = end >= GetSegmentEnd(start_segment);

            bool should_erase_segment = at_start && at_end;

            if(should_erase_segment) {
                _segments.erase(_segments.begin() + start_index);
                return;
            }

            InsertSegment(SplitSegment(start_segment, start - start_segment._start, end - start + 1), start_index + 1);
        } else {
            bool should_erase_start = start <= start_segment._start;
            bool should_erase_end   = end >= GetSegmentEnd(end_segment);

            auto erase_start_iter = _segments.begin() + start_index + !should_erase_start;
            auto erase_end_iter   = _segments.begin() + end_index + should_erase_end;

            _segments.erase(erase_start_iter, erase_end_iter);

            if(!should_erase_start) {
                SegmentErase(start_segment, start - start_segment._start);
            }

            if(!should_erase_end) {
                SegmentErase(end_segment, 0, end - end_segment._start + 1);
            }
        }

        PushSegments(start_index + 1, -len);
    }

    /**
     * @brief Inserts a segment at the index(g), moves following segments back if there is no space
     * @param str The contents of the new segment
     * @param style The style of the segment
     * @param index The index(g) where the segment should be inserted
     */
    void Insert(const tiny_utf8::string& str, const Style& style, uint64_t index) {
        if(index > Len()) {
            throw std::range_error("Index is out of bounds!");
        }

        uint64_t segment_index = GetSegmentIndex(index);
        Segment& segment       = *_segments[segment_index];

        PushSegments(segment_index + 1, str.length());

        // If the style is the same as the style of the segment we can just insert the string into the string of the
        // segment and return early.
        bool can_insert = style == segment._style;
        if(can_insert) {
            segment._str.insert(index - segment._start, str);
            return;
        }

        _SegmentPtr split = SplitSegment(segment, index - segment._start, 0);
        split->_start += str.length();

        InsertSegment(std::move(split), segment_index + 1);
        InsertSegment(CreateSegment(str, style, index), segment_index + 1);
    }

    struct Reference {
        friend _type;

      public:
        void operator=(const StyledChar& character) {
            _src.Set(character, _index);
        }

      private:
        Reference(_type& src, uint64_t index) : _src(src) {
            _index = index;
        }

        _type&   _src;
        uint64_t _index;
    };

    /**
     * @brief Sets the character at the index
     * @param character The value to set to
     * @param index The index(g)
     */
    void Set(const StyledChar& character, uint64_t index) {
        if(index >= Len()) {
            throw std::range_error("Index is out of bounds!");
        }

        uint64_t segment_index = GetSegmentIndex(index);
        Segment& segment       = *_segments[segment_index];

        // If the segment at the index and the character have the same style we can just set the character at that index
        // inside the string, and return early.
        bool can_set = segment._style == character._style;
        if(can_set) {
            segment._str[index - segment._start] = character._char;
            return;
        }

        _SegmentPtr new_seg = CreateSegment(character, index);
        _SegmentPtr split   = SplitSegment(segment, index, 1);

        InsertSegment(std::move(split), segment_index);
        InsertSegment(std::move(new_seg), segment_index);
    }

    Reference operator[](uint64_t index) {
        if(index >= Len()) {
            throw std::range_error("Index is out of bounds! << StyledString<true>::operator[]()");
        }

        return Reference(*this, index);
    }

    /**
     * @brief Gets the character at the index
     * @param index The index(g)
     */
    StyledChar CharAt(uint64_t index) const {
        if(index >= Len()) {
            throw std::range_error("Index is out of bounds! << StyledString<true>::CharAt()");
        }

        const Segment& segment = *_segments[GetSegmentIndex(index)];

        return StyledChar(segment._str[index - segment._start], segment._style);
    }
};

} // namespace LibTesix