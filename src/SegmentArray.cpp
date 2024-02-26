#include "SegmentArray.h"

#include <stdexcept>

namespace LibTesix {

SegmentArray::StyledSegment::StyledSegment(const icu::UnicodeString& str, const Style* style, uint64_t start) {
    this->_str = icu::UnicodeString(str);
    this->_style = style;
    this->_start = start;
}

SegmentArray::StyledSegment::StyledSegment(const char* str, const Style* style, uint64_t start) {
    this->_str = icu::UnicodeString(str);
    this->_style = style;
    this->_start = start;
}

SegmentArray::StyledSegment::StyledSegment() {
    _str = icu::UnicodeString("");

    _style = STANDARD_STYLE;
    _start = 0;
}

SegmentArray::StyledSegment SegmentArray::StyledSegment::Split(uint64_t index) {
    if(index > _str.length()) throw std::runtime_error("Index " + std::to_string(index) + " is out of bounds! << StyledSegment::Split()");

    icu::UnicodeString substr(_str, index);

    _str.remove(index);

    return StyledSegment(substr, _style, _start + index);
}

uint64_t SegmentArray::StyledSegment::Len() const {
    return _str.length();
}

SegmentArray::SegmentArray() {
}

bool SegmentArray::InSegment(uint64_t segment_index, uint64_t index) const {
    return (index < _segments[segment_index]._start + _segments[segment_index].Len()) && (index >= _segments[segment_index]._start);
}

void SegmentArray::Append(const icu::UnicodeString& str, const Style* style) {
    uint64_t segment_start = 0;

    if(_segments.size() > 0) {
        segment_start = _segments.back()._start + _segments.back().Len();
    }

    _segments.emplace_back(str, style, segment_start);
}

void SegmentArray::Append(const char* str, const Style* style) {
    icu::UnicodeString uc_str(str);
    Append(uc_str, style);
}

void SegmentArray::Add(const icu::UnicodeString& str, const Style* style, uint64_t index) {
    // If nothing to add return
    if(str.length() == 0) return;

    StyledSegment new_segment = StyledSegment(str, style, index);

    // If no segments present just append
    if(_segments.size() == 0) {
        InsertSegment(new_segment, 0);
        return;
    }

    // If the SegmentArray is empty just set the segment
    if(Len() == 0) _segments[0] = new_segment;

    uint64_t segment_index = GetSegmentIndex(index);

    if(index >= Len()) {
        // If the index is greater than the SegmentArray
        _segments.push_back(new_segment);
    } else if(index + str.length() >= Len()) {
        // If the index is in the last segment or beyond

        Erase(index, Len() - 1);

        _segments.push_back(new_segment);
    } else {
        // If no other conditions are met the index must be before any segment or in one
        bool back = _segments[segment_index]._start < index;

        Erase(index, index + str.length() - 1);

        InsertSegment(new_segment, segment_index + back);
    }

    Merge(segment_index >= 1 ? segment_index : 0, segment_index + 1);
}

void SegmentArray::Add(const char* str, const Style* style, uint64_t index) {
    icu::UnicodeString uc_str(str);
    Add(uc_str, style, index);
}

void SegmentArray::Add(const StyledChar& character, uint64_t index) {
    UChar32 foo = (UChar32)(character._character);

    icu::UnicodeString uc_str(foo);
    Add(uc_str, character._style, index);
}

void SegmentArray::Erase(uint64_t start, uint64_t end) {
    if(start >= Len()) throw std::runtime_error("Index of start: " + std::to_string(start) + " is out of bounds << StyledSegmentArray::Erase()");
    if(end >= Len()) throw std::runtime_error("Index of end: " + std::to_string(end) + " is out of bounds << StyledSegmentArray::Erase()");

    if(start > end) std::swap(start, end);

    if(!HitsSegment(start, end)) return;

    uint64_t start_segment_index = GetSegmentIndex(start);
    uint64_t end_segment_index = GetSegmentIndex(end);

    StyledSegment& start_segment = _segments[start_segment_index];

    // if erasing
    if(start < start_segment._start) {
        uint64_t erase_len = (end - start) - (start_segment._start - start) + 1;

        start_segment._str.remove(0, erase_len);
        start_segment._start += erase_len;

        // Clean up
        Clean(start_segment_index);
    } else if(start_segment_index == end_segment_index) {
        // Remove text
        uint64_t erase_index = start - start_segment._start;
        uint64_t erase_len = end - start + 1;

        start_segment._str.remove(erase_index, erase_len);

        // Split initial segment
        InsertSegment(start_segment.Split(erase_index), start_segment_index + 1);
        _segments[start_segment_index + 1]._start += erase_len;

        // Clean up
        Clean(start_segment_index + !Clean(start_segment_index));
    } else {
        // Remove text in start segment
        start_segment.Split(start - start_segment._start);

        // Remove everything inbetween
        for(uint64_t i = start_segment_index + 1; i < end_segment_index;) {
            _segments.erase(_segments.begin() + i);
            end_segment_index--;
        }

        StyledSegment& end_segment = _segments[end_segment_index];

        // Remove text in end segment
        end_segment = end_segment.Split(end - end_segment._start + 1);

        // Clean up
        Clean(end_segment_index);
        Clean(start_segment_index);
    }
}

void SegmentArray::Clear() {
    _segments.clear();
}

void SegmentArray::InsertSegment(StyledSegment segment, uint64_t index) {
    if(index == _segments.size()) {
        _segments.push_back(segment);
    } else {
        _segments.insert(_segments.begin() + index, segment);
    }
}

#ifdef NDEBUG
void SegmentArray::PrintDebug() const {
    printf("|");
    for(const StyledSegment& seg : _segments) {
        std::string utf8;
        seg._str.toUTF8String(utf8);
        printf("%i \"%s\"|", seg._start, utf8.c_str());
    }
    printf("%i|\n", Len());
}
#endif

SegmentArray::StyledChar SegmentArray::operator[](uint64_t index) const {
    uint64_t segment_index = GetSegmentIndex(index);

    if(!InSegment(segment_index, index)) {
        throw std::out_of_range("Index is out of bounds or not within a segment! << SegmentArray::operator[]");
    }

    const StyledSegment& segment = _segments[GetSegmentIndex(index)];

    const Style* style = segment._style;
    char32_t character = segment._str.char32At(index - segment._start);

    return StyledChar(character, style);
}

const Style* SegmentArray::GetStyleStart() const {
    if(_segments.size() > 0) {
        return _segments[0]._style;
    } else {
        return nullptr;
    }
}

const Style* SegmentArray::GetStyleEnd() const {
    if(_segments.size() > 0) {
        return _segments.back()._style;
    } else {
        return nullptr;
    }
}

uint64_t SegmentArray::Len() const {
    if(_segments.size() == 0) {
        return 0;
    } else {
        return _segments.back()._start + _segments.back().Len();
    }
}

uint64_t SegmentArray::GetSegmentIndex(uint64_t index) const {
    size_t segment_index = 0;

    if(_segments.size() == 0) {
        return 0;
    }

    if(index >= _segments.back()._start) {
        return _segments.size() - 1;
    }

    for(size_t i = 0; i < _segments.size() - 1; i++) {
        if(index < _segments[i + 1]._start) {
            segment_index = i;

            if(_segments[i + 1]._start == _segments[i]._start) {
                continue;
            } else {
                break;
            }
        }
    }

    return segment_index;
}

bool SegmentArray::Clean(uint64_t segment_index) {
    if(_segments[segment_index].Len() == 0) {
        _segments.erase(_segments.begin() + segment_index);
        return true;
    }

    return false;
}

bool SegmentArray::HitsSegment(uint64_t start, uint64_t end) const {
    uint64_t start_segment_index = GetSegmentIndex(start);
    uint64_t end_segment_index = GetSegmentIndex(end);
    return start_segment_index != end_segment_index || InSegment(start_segment_index, start) || InSegment(end_segment_index, end);
}

void SegmentArray::Merge(uint64_t start, uint64_t end) {
    uint64_t len = std::min(end, _segments.size()) - start;

    for(uint64_t i = start; i < len;) {
        if(i + 1 >= _segments.size()) break;

        StyledSegment& first = _segments[i];
        StyledSegment& second = _segments[i + 1];

        if(first._style == second._style) {
            first._str.append(second._str);

            _segments.erase(_segments.begin() + i + 1);
        } else {
            i++;
        }
    }
}

SegmentArray::StyledChar::StyledChar(char32_t character, const Style* style) : _character(character), _style(style) {
}

bool SegmentArray::StyledChar::operator==(const StyledChar& other) const {
    return other._character == _character && other._style == _style;
}

} // namespace LibTesix