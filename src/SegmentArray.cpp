#include "SegmentArray.h"

#include <stdexcept>

namespace LibTesix {

StyledSegment::StyledSegment(const icu::UnicodeString& str, const Style* style, uint64_t start) {
    this->str = icu::UnicodeString(str);
    this->style = style;
    this->start = start;
}

StyledSegment::StyledSegment(const char* str, const Style* style, uint64_t start) {
    this->str = icu::UnicodeString(str);
    this->style = style;
    this->start = start;
}

StyledSegment::StyledSegment() {
    str = icu::UnicodeString("");

    style = style_allocator[0UL];
    start = 0;
}

std::shared_ptr<StyledSegment> StyledSegment::Split(uint64_t index) {
    if(index > str.length()) throw std::runtime_error("Index " + std::to_string(index) + " is out of bounds! << StyledSegment::Split()");

    icu::UnicodeString substr(str, index);

    str.remove(index);

    return std::make_shared<StyledSegment>(substr, style, start + index);
}

uint64_t StyledSegment::Len() const {
    return str.length();
}

StyledSegmentArray::StyledSegmentArray() {
}

bool StyledSegmentArray::InSegment(uint64_t segment_index, uint64_t index) const {
    return (index < segments[segment_index].get()->start + segments[segment_index].get()->Len()) && (index >= segments[segment_index].get()->start);
}

void StyledSegmentArray::Append(const icu::UnicodeString& str, const Style* style) {
    uint64_t segment_start = 0;

    if(segments.size() > 0) {
        segment_start = segments.back().get()->start + segments.back().get()->Len();
    }

    segments.push_back(std::make_shared<StyledSegment>(str, style, segment_start));
}

void StyledSegmentArray::Append(const char* str, const Style* style) {
    icu::UnicodeString uc_str(str);
    Append(uc_str, style);
}

void StyledSegmentArray::Add(const icu::UnicodeString& str, const Style* style, uint64_t index) {
    if(str.length() == 0) return;

    std::shared_ptr<StyledSegment> new_segment = std::make_shared<StyledSegment>(str, style, index);

    if(segments.size() == 0) {
        InsertSegment(new_segment, 0);
        return;
    }

    if(Len() == 0) segments[0] = new_segment;

    if(index >= Len()) {
        segments.push_back(new_segment);
    } else if(index + str.length() >= Len()) {
        Erase(index, Len() - 1);

        segments.push_back(new_segment);
    } else {
        uint64_t segment_index = GetSegmentIndex(index);
        bool back = !((segments[segment_index].get()->start == index) || !(segments[segment_index].get()->start < index));

        Erase(index, index + str.length() - 1);

        InsertSegment(new_segment, segment_index + back);
    }
}

void StyledSegmentArray::Add(const char* str, const Style* style, uint64_t index) {
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

    if(start < segments[start_segment_index].get()->start) {
        // Remove text
        uint64_t erase_len = (end - start + 1) - (segments[start_segment_index].get()->start - start);

        segments[start_segment_index].get()->str.remove(0, erase_len);
        segments[start_segment_index].get()->start += erase_len;

        // Clean up
        Clean(start_segment_index);
    } else if(start_segment_index == end_segment_index) {
        // Remove text
        uint64_t erase_index = start - segments[start_segment_index].get()->start;
        uint64_t erase_len = end - start + 1;

        segments[start_segment_index].get()->str.remove(erase_index, erase_len);

        // Split initial segment
        InsertSegment(segments[start_segment_index].get()->Split(erase_index), start_segment_index + 1);
        segments[start_segment_index + 1].get()->start += erase_len;

        // Clean up
        Clean(start_segment_index + !Clean(start_segment_index));
    } else {
        bool clean_start;
        bool clean_end;

        // Remove text in start segment
        segments[start_segment_index].get()->Split(start - segments[start_segment_index].get()->start);
        clean_start = segments[start_segment_index].get()->Len() == 0;

        // Remove everything inbetween
        for(uint64_t i = start_segment_index + 1; i < end_segment_index;) {
            segments.erase(segments.begin() + i);
            end_segment_index--;
        }

        // Remove text in end segment
        segments[end_segment_index] = segments[end_segment_index].get()->Split(end - segments[end_segment_index].get()->start + 1);
        clean_end = segments[end_segment_index].get()->Len() == 0;

        // Clean up
        if(clean_end) Clean(end_segment_index);
        if(clean_start) Clean(start_segment_index);
    }
}

void StyledSegmentArray::Clear() {
    segments.clear();
}

void StyledSegmentArray::InsertSegment(std::shared_ptr<StyledSegment> segment, uint64_t index) {
    if(index == segments.size()) {
        segments.push_back(segment);
    } else {
        segments.insert(segments.begin() + index, segment);
    }
}

void StyledSegmentArray::PrintDebug() const {
    printf("|");
    for(std::shared_ptr<StyledSegment> seg : segments) {
        std::string utf8;
        seg.get()->str.toUTF8String(utf8);
        printf("%i \"%s\"|", seg.get()->start, utf8.c_str());
    }
    printf("%i|\n", Len());
}

uint64_t StyledSegmentArray::Len() const {
    if(segments.size() == 0) {
        return 0;
    } else {
        return segments.back().get()->start + segments.back().get()->Len();
    }
}

uint64_t StyledSegmentArray::GetSegmentIndex(uint64_t index) const {
    size_t segment_index = 0;

    if(segments.size() == 0) {
        return 0;
    }

    if(index >= segments.back().get()->start) {
        return segments.size() - 1;
    }

    for(size_t i = 0; i < segments.size() - 1; i++) {
        if(index < segments[i + 1].get()->start) {
            segment_index = i;

            if(segments[i + 1].get()->start == segments[i].get()->start) {
                continue;
            } else {
                break;
            }
        }
    }

    return segment_index;
}

bool StyledSegmentArray::Clean(uint64_t segment_index) {
    if(segments[segment_index].get()->Len() == 0) {
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
