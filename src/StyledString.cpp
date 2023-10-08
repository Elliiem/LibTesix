#include "StyledString.h"

#include <iostream>
#include <stdexcept>

namespace LibTesix {

StyledString::StyledSegment::StyledSegment(std::string str, Style style, uint32_t start) {
    this->str = str;
    this->style = style;
    this->start = start;
}

StyledString::StyledSegment StyledString::StyledSegment::Split(uint32_t index) {
    if(!(index < str.length())) return *this;
    StyledSegment new_segment = StyledSegment(str.substr(index), style, start + index);

    str = str.substr(0, index);

    return new_segment;
}

StyledString::StyledSegment StyledString::StyledSegment::Combine(
    std::vector<StyledSegment> segments) {
    if(segments.size() == 0)
        throw std::runtime_error(
            "Provided vector is empty! << StyledString::StyledSegment StyledString::StyledSegment::Combine()");
    StyledSegment ret("", STANDARD_STYLE, segments[0].start);
    for(StyledSegment segment : segments) {
        ret.str.append(segment.str);
    }

    return ret;
}

StyledString::StyledString(std::string base_string) {
    string.push_back(StyledSegment(base_string, STANDARD_STYLE));
    raw_string = "\033[38;2;255;255;255m\033[48;2;0;0;0m" + base_string;
}

StyledString::StyledString(std::vector<StyledSegment> string) {
    this->string = string;
    UpdateRaw();
}

void StyledString::Insert(std::string str, Style style, uint32_t index) {
    uint32_t start = Split(index).first;

    string.insert(string.begin() + start + 1, StyledSegment(str, style, index));

    for(uint32_t i = start + 2; i < string.size(); i++) {
        string[i].start += str.size();
    }
}

std::string StyledString::Write(std::string str, Style style, uint32_t index) {
    uint32_t len = Len();
    if(index >= len)
        throw std::runtime_error(
            "Index " + std::to_string(index) + " is out of bounds! << StyledString::Write()");

    uint32_t start = Split(index).second;
    uint32_t end = Split(index + str.length()).first;

    uint32_t start_index = string[start].start;

    while(start != end + 1) {
        string.erase(string.begin() + start);
        end--;
    }

    std::string cutoff;
    uint32_t cutoff_index = len - start_index;
    uint32_t str_len = str.length();
    if(cutoff_index < str.length()) {
        cutoff = str.substr(cutoff_index);
        str.resize(len - start_index);
    }

    StyledSegment new_segment = StyledSegment(str, style, start_index);
    string.insert(string.begin() + start, new_segment);
    return cutoff;
}

void StyledString::Append(std::string str, Style style) {
    StyledSegment& last = string.back();
    string.push_back(StyledSegment(str, style, Len()));
}

void StyledString::Erase(uint32_t start, uint32_t end) {
    if(start > Len())
        throw std::runtime_error("Index of start " + std::to_string(start) +
                                 " is out of bounds! << StyledString::Erase()");
    if(end > Len())
        throw std::runtime_error(
            "Index of end " + std::to_string(end) + " is out of bounds! << StyledString::Erase()");

    uint32_t start_segment_index = GetSegmentIndex(start);
    uint32_t end_segment_index = GetSegmentIndex(end);

    StyledSegment cur = string[start_segment_index];

    if(start_segment_index == end_segment_index) {
        uint32_t erase_start = start - cur.start;
        uint32_t erase_len = end - cur.start - erase_start;
        cur.str.erase(erase_start, erase_len);
    } else {
        cur.Split(start - cur.start);

        if(cur.str == "") {
            string.erase(string.begin() + start_segment_index);
            end_segment_index--;
        }

        for(uint32_t i = start_segment_index; i < end_segment_index; i++) {
            string.erase(string.begin() + i);
        }

        cur = string[start_segment_index];

        string[start_segment_index] = cur.Split(end - cur.start);

        UpdateSegmentStart();
    }
}

void StyledString::UpdateRaw() {
    std::string new_raw("");

    Style state = string[0].style;

    for(StyledSegment segment : string) {
        new_raw.append(segment.style.GetEscapeCode(&state));
        new_raw.append(segment.str);
        state = segment.style;
    }

    raw_string = new_raw;
}

void StyledString::UpdateSegmentStart() {
    uint32_t next_start = 0;

    uint32_t i = 0;
    while(i <= string.size()) {
        string[i].start = next_start;
        next_start = string[i].str.length();
        i++;
    }

    string[i].start = next_start;
}

uint32_t StyledString::GetSegmentIndex(uint32_t index) {
    uint32_t i = 1;
    while(index > string[i].start && i != string.size()) {
        i++;
    }

    return i - 1;
}

std::pair<uint32_t, uint32_t> StyledString::Split(uint32_t index) {
    uint32_t segment_index = GetSegmentIndex(index);
    StyledSegment* segment = &string[segment_index];

    if(segment->start == index) return std::pair<uint32_t, uint32_t>(segment_index, segment_index);
    else if(segment->start + segment->str.length() <= index)
        return std::pair<uint32_t, uint32_t>(segment_index, segment_index);

    StyledSegment new_segment = segment->Split(index - segment->start);

    string.insert(string.begin() + segment_index + 1, new_segment);

    return std::pair<uint32_t, uint32_t>(segment_index, segment_index + 1);
}

uint32_t StyledString::Len() {
    StyledSegment& last = string.back();
    return last.start + last.str.length();
}

std::string& StyledString::Raw(Style* state) {
    raw_string = string[0].style.GetEscapeCode(state) + raw_string;
    return raw_string;
}

Style StyledString::StyleStart() {
    return string[0].style;
}

Style StyledString::StyleEnd() {
    return string[string.size() - 1].style;
}

StyledString StyledString::Substr(uint32_t start, uint32_t end) {
    if(start > Len())
        throw std::runtime_error("Index of start " + std::to_string(start) +
                                 " is out of bounds! << StyledString::Substr()");
    if(end > Len())
        throw std::runtime_error(
            "Index of end " + std::to_string(end) + " is out of bounds! << StyledString::Substr()");

    uint32_t start_segment_index = GetSegmentIndex(start);
    uint32_t end_segment_index = GetSegmentIndex(end);

    std::vector<StyledSegment> substr;

    if(start_segment_index == end_segment_index) {
        StyledSegment segment = string[start_segment_index];
        substr.push_back(StyledSegment(
            segment.str.substr(start - segment.start, end - segment.start), segment.style, 0));
    } else {
        StyledSegment start_segment = string[start_segment_index];
        substr.push_back(StyledSegment(
            start_segment.str.substr(start - start_segment.start), start_segment.style, 0));

        for(uint32_t i = start_segment_index + 1; i < end_segment_index; i++) {
            substr.push_back(string[i]);
        }

        StyledSegment end_segment = string[end_segment_index];
        substr.push_back(StyledSegment(end_segment.str.substr(0, end_segment.str.length()),
            end_segment.style, substr.back().start + 1));
    }

    return StyledString(substr);
}

} // namespace LibTesix