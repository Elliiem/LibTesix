#include "SegmentArray.h"

#include <stdexcept>

LibTesix::StyledSegmentArray::StyledSegmentArray() {
}

bool LibTesix::StyledSegmentArray::InSegment(uint32_t segment_index, uint32_t index) {
    return (index < segments[segment_index].start + segments[segment_index].str.length()) && (index >= segments[segment_index].start);
}

void LibTesix::StyledSegmentArray::Append(const icu::UnicodeString& str, Style style) {
    uint32_t segment_start = 0;

    if(segments.size() > 0) {
        segment_start = segments.back().start + segments.back().str.length();
    }

    segments.push_back(StyledSegment(str, style, segment_start));
}

void LibTesix::StyledSegmentArray::Append(const char* str, Style style) {
    icu::UnicodeString uc_str(str);
    Append(uc_str, style);
}

void LibTesix::StyledSegmentArray::Insert(const icu::UnicodeString& str, Style style, uint32_t index) {
    if(str.length() == 0) return;

    StyledSegment new_segment(str, style, index);

    if(index >= Len()) {
        segments.push_back(new_segment);
    } else if(index + str.length() >= Len()) {
        Erase(index, Len() - 1);

        segments.push_back(new_segment);
    } else {
        printf("a\n");
        uint32_t segment_index = GetSegmentIndex(index);
        bool back = !((segments[segment_index].start == index) || !(segments[segment_index].start < index));

        printf("%i, %i\n", (segments[segment_index].start == index), !(segments[segment_index].start < index));

        Erase(index, index + str.length() - 1);

        InsertSegment(new_segment, segment_index + back);
    }
}

void LibTesix::StyledSegmentArray::Insert(const char* str, Style style, uint32_t index) {
    icu::UnicodeString uc_str(str);
    Insert(uc_str, style, index);
}

void LibTesix::StyledSegmentArray::Erase(uint32_t start, uint32_t end) {
    if(start >= Len()) throw std::runtime_error("Index of start: " + std::to_string(start) + " is out of bounds << StyledSegmentArray::Erase()");
    if(end >= Len()) throw std::runtime_error("Index of end: " + std::to_string(end) + " is out of bounds << StyledSegmentArray::Erase()");

    if(start > end) std::swap(start, end);

    uint32_t start_segment_index = GetSegmentIndex(start);
    uint32_t end_segment_index = GetSegmentIndex(end);

    if(start < segments[start_segment_index].start) {
        uint32_t erase_len = (end - start + 1) - (segments[start_segment_index].start - start);

        segments[start_segment_index].str.remove(0, erase_len);

        Clean(start_segment_index);
    } else if(start_segment_index == end_segment_index) {
        uint32_t erase_index = start - segments[start_segment_index].start;
        uint32_t erase_len = end - start + 1;

        // Remove text
        segments[start_segment_index].str.remove(erase_index, erase_len);

        // Split segment to start and end
        InsertSegment(segments[start_segment_index].Split(erase_index), start_segment_index + 1);
        segments[start_segment_index + 1].start += erase_len;

        // Clean up
        Clean(start_segment_index + !Clean(start_segment_index));
    } else {
        bool clean_start;
        bool clean_end;

        // Remove text in start segment
        segments[start_segment_index].Split(start - segments[start_segment_index].start);
        clean_start = segments[start_segment_index].str.length() == 0;

        // Remove everything inbetween
        for(uint32_t i = start_segment_index + 1; i < end_segment_index; i++) {
            segments.erase(segments.begin() + i);
            end_segment_index--;
        }

        // Remove text in end segment
        segments[end_segment_index] = segments[end_segment_index].Split(end - segments[end_segment_index].start + 1);
        clean_end = segments[end_segment_index].str.length() == 0;

        // Clean up
        if(clean_end) Clean(end_segment_index);
        if(clean_start) Clean(start_segment_index);
    }
}

void LibTesix::StyledSegmentArray::InsertSegment(StyledSegment segment, uint32_t index) {
    if(index == segments.size()) {
        segments.push_back(segment);
    } else {
        segments.insert(segments.begin() + index, segment);
    }
}

void LibTesix::StyledSegmentArray::PrintDebug() {
    printf("|");
    for(StyledSegment seg : segments) {
        std::string utf8;
        seg.str.toUTF8String(utf8);
        printf("%i(%s)|", seg.start, utf8.c_str());
    }
    printf("%i|\n", Len());
}

uint32_t LibTesix::StyledSegmentArray::Len() {
    if(segments.size() == 0) {
        return 0;
    } else {
        return segments.back().start + segments.back().str.length();
    }
}

uint32_t LibTesix::StyledSegmentArray::GetSegmentIndex(uint32_t index) {
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

bool LibTesix::StyledSegmentArray::Clean(uint32_t segment_index) {
    if(segments[segment_index].str.length() == 0) {
        segments.erase(segments.begin() + segment_index);
        return true;
    }

    return false;
}
