#include "SegmentArray.h"

#include <stdexcept>

namespace LibTesix {

StyledSegment::StyledSegment(const icu::UnicodeString& str, Style style, uint64_t start) {
    this->str = icu::UnicodeString(str);
    this->style = style;
    this->start = start;
}

StyledSegment::StyledSegment(const char* str, Style style, uint64_t start) {
    this->str = icu::UnicodeString(str);
    this->style = style;
    this->start = start;
}

StyledSegment::StyledSegment() {
    str = icu::UnicodeString("");

    style = STANDARD_STYLE;
    start = 0;
}

StyledSegment StyledSegment::Split(uint64_t index) {
    if(index > str.length()) throw std::runtime_error("Index " + std::to_string(index) + " is out of bounds! << StyledSegment::Split()");

    icu::UnicodeString substr(str, index);

    StyledSegment new_seg(substr, style, start + index);

    str.remove(index);

    return new_seg;
}

uint64_t StyledSegment::Len() const {
    return str.length();
}

StyledSegmentArray::StyledSegmentArray() {
}

bool StyledSegmentArray::InSegment(uint64_t segment_index, uint64_t index) const {
    return (index < segments[segment_index].start + segments[segment_index].Len()) && (index >= segments[segment_index].start);
}

void StyledSegmentArray::Append(const icu::UnicodeString& str, Style style) {
    uint64_t segment_start = 0;

    if(segments.size() > 0) {
        segment_start = segments.back().start + segments.back().Len();
    }

    segments.push_back(StyledSegment(str, style, segment_start));
}

void StyledSegmentArray::Append(const char* str, Style style) {
    icu::UnicodeString uc_str(str);
    Append(uc_str, style);
}

void StyledSegmentArray::Add(const icu::UnicodeString& str, const Style& style, uint64_t index) {
    if(str.length() == 0) return;

    StyledSegment new_segment(str, style, index);

    if(segments.size() == 0) {
        InsertSegment(StyledSegment(str, style, index), 0);
    }

    if(Len() == 0) segments[0] = new_segment;

    if(index >= Len()) {
        segments.push_back(new_segment);
    } else if(index + str.length() >= Len()) {
        Erase(index, Len() - 1);

        segments.push_back(new_segment);
    } else {
        uint64_t segment_index = GetSegmentIndex(index);
        bool back = !((segments[segment_index].start == index) || !(segments[segment_index].start < index));

        Erase(index, index + str.length() - 1);

        InsertSegment(new_segment, segment_index + back);
    }
}

void StyledSegmentArray::Add(const char* str, const Style& style, uint64_t index) {
    icu::UnicodeString uc_str(str);
    Add(uc_str, style, index);
}

void StyledSegmentArray::Erase(uint64_t start, uint64_t end) {
    if(start >= Len()) throw std::runtime_error("Index of start: " + std::to_string(start) + " is out of bounds << StyledSegmentArray::Erase()");
    if(end >= Len()) throw std::runtime_error("Index of end: " + std::to_string(end) + " is out of bounds << StyledSegmentArray::Erase()");

    if(start > end) std::swap(start, end);

    if(!HitsSegment(start, end)) return;

    uint64_t start_segment_index = GetSegmentIndex(start);
    uint64_t end_segment_index = GetSegmentIndex(end);

    if(start < segments[start_segment_index].start) {
        // Remove text
        uint64_t erase_len = (end - start + 1) - (segments[start_segment_index].start - start);

        segments[start_segment_index].str.remove(0, erase_len);
        segments[start_segment_index].start += erase_len;

        // Clean up
        Clean(start_segment_index);
    } else if(start_segment_index == end_segment_index) {
        // Remove text
        uint64_t erase_index = start - segments[start_segment_index].start;
        uint64_t erase_len = end - start + 1;

        segments[start_segment_index].str.remove(erase_index, erase_len);

        // Split initial segment
        InsertSegment(segments[start_segment_index].Split(erase_index), start_segment_index + 1);
        segments[start_segment_index + 1].start += erase_len;

        // Clean up
        Clean(start_segment_index + !Clean(start_segment_index));
    } else {
        bool clean_start;
        bool clean_end;

        // Remove text in start segment
        segments[start_segment_index].Split(start - segments[start_segment_index].start);
        clean_start = segments[start_segment_index].Len() == 0;

        // Remove everything inbetween
        for(uint64_t i = start_segment_index + 1; i < end_segment_index;) {
            segments.erase(segments.begin() + i);
            end_segment_index--;
        }

        // Remove text in end segment
        segments[end_segment_index] = segments[end_segment_index].Split(end - segments[end_segment_index].start + 1);
        clean_end = segments[end_segment_index].Len() == 0;

        // Clean up
        if(clean_end) Clean(end_segment_index);
        if(clean_start) Clean(start_segment_index);
    }
}

void StyledSegmentArray::Clear() {
    segments.clear();
}

void StyledSegmentArray::InsertSegment(StyledSegment segment, uint64_t index) {
    if(index == segments.size()) {
        segments.push_back(segment);
    } else {
        segments.insert(segments.begin() + index, segment);
    }
}

void StyledSegmentArray::PrintDebug() const {
    printf("|");
    for(StyledSegment seg : segments) {
        std::string utf8;
        seg.str.toUTF8String(utf8);
        printf("%i \"%s\"|", seg.start, utf8.c_str());
    }
    printf("%i|\n", Len());
}

uint64_t StyledSegmentArray::Len() const {
    if(segments.size() == 0) {
        return 0;
    } else {
        return segments.back().start + segments.back().Len();
    }
}

uint64_t StyledSegmentArray::GetSegmentIndex(uint64_t index) const {
    size_t segment_index = 0;

    if(segments.size() == 0) {
        return 0;
    }

    if(index >= segments.back().start) {
        return segments.size() - 1;
    }

    for(size_t i = 0; i < segments.size() - 1; i++) {
        if(index < segments[i + 1].start) {
            segment_index = i;

            if(segments[i + 1].start == segments[i].start) {
                continue;
            } else {
                break;
            }
        }
    }

    return segment_index;
}

bool StyledSegmentArray::Clean(uint64_t segment_index) {
    if(segments[segment_index].Len() == 0) {
        segments.erase(segments.begin() + segment_index);
        return true;
    }

    return false;
}

bool StyledSegmentArray::HitsSegment(uint64_t start, uint64_t end) const {
    uint64_t start_segment_index = GetSegmentIndex(start);
    uint64_t end_segment_index = GetSegmentIndex(end);
    return start_segment_index != end_segment_index || InSegment(start_segment_index, start) || InSegment(end_segment_index, end);
}

} // namespace LibTesix
