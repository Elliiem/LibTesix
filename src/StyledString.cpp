#include "StyledString.h"

#include <iostream>
#include <stdexcept>

namespace LibTesix {

StyledString::StyledSegment::StyledSegment(std::string str, Style style, uint32_t start) {
    this->str = str;
    this->style = style;
    this->start = start;
}

StyledString::StyledSegment::StyledSegment() {
    str = "";
    style = STANDARD_STYLE;
    start = 0;
}

StyledString::StyledSegment StyledString::StyledSegment::Split(uint32_t index) {
    if(!(index < str.length())) return *this;
    StyledSegment new_segment = StyledSegment(str.substr(index), style, start + index);

    str = str.substr(0, index);

    return new_segment;
}

StyledString::StyledSegment StyledString::StyledSegment::Combine(std::vector<StyledSegment> segments) {
    if(segments.size() == 0)
        throw std::runtime_error("Provided vector is empty! << StyledString::StyledSegment StyledString::StyledSegment::Combine()");
    StyledSegment ret("", STANDARD_STYLE, segments[0].start);
    for(StyledSegment segment : segments) {
        ret.str.append(segment.str);
    }

    return ret;
}

StyledString::StyledString(std::string base_string) {
    string.push_back(StyledSegment(base_string, STANDARD_STYLE));
    raw = "\033[38;2;255;255;255m\033[48;2;0;0;0m" + base_string;
}

StyledString::StyledString(std::vector<StyledSegment> string) {
    this->string = string;
    UpdateRaw();
}

void StyledString::Insert(std::string str, Style style, uint32_t index) {
    uint32_t start_segment_index = GetSegmentIndex(index);
    StyledSegment seg = string[start_segment_index].Split(index - string[start_segment_index].start);
    string.insert(string.begin() + start_segment_index + 1, seg);

    string.insert(string.begin() + start_segment_index + 1, StyledSegment(str, style, index));

    UpdateSegmentStart(start_segment_index + 1);
}

std::string StyledString::Write(std::string str, Style style, uint32_t index) {
    BoundsCheck(index + 1, "Index " + std::to_string(index) + " is out of bounds! << StyledString::Write()");

    std::string ret;
    int32_t over = str.length() + index - Len();
    if(over > 0) {
        ret = str.substr(str.length() - over);
        str.resize(str.length() - over);
    }

    Erase(index, index + str.length());

    uint32_t start_segment_index = GetSegmentIndex(index);
    StyledSegment seg(str, style, string[start_segment_index].start + string[start_segment_index].str.length());

    if(string.size() > 1) {
        string.insert(string.begin() + start_segment_index + 1, seg);
    } else {
        string.insert(string.begin(), seg);
    }

    UpdateSegmentStart(start_segment_index);
    return ret;
}

void StyledString::Append(std::string str, Style style) {
    if(string.size() == 1 && string[0].str == "") {
        string[0] = StyledSegment(str, style, Len());
    } else {
        string.push_back(StyledSegment(str, style, Len()));
    }
}

void StyledString::Erase(uint32_t start, uint32_t end) {
    BoundsCheck(start, "Index of start " + std::to_string(start) + " is out of bounds! << StyledString::Erase()");
    BoundsCheck(end, "Index of end " + std::to_string(end) + " is out of bounds! << StyledString::Erase()");

    uint32_t start_segment_index = GetSegmentIndex(start);
    uint32_t end_segment_index = GetSegmentIndex(end);

    StyledSegment* cur = &string[start_segment_index];
    UpdateSegmentStart(0);

    if(start_segment_index == end_segment_index) {
        uint32_t erase_start = start - cur->start;
        uint32_t erase_len = end - cur->start - erase_start;

        cur->str.erase(erase_start, erase_len);

        if(cur->str == "" && string.size() > 1) {
            string.erase(string.begin() + start_segment_index);
        }
    } else {
        cur->Split(start - cur->start);
        if(cur->str == "") {
            string.erase(string.begin() + start_segment_index);
            end_segment_index--;
        } else {
            start_segment_index++;
        }

        for(uint32_t i = start_segment_index; i < end_segment_index; i++) {
            string.erase(string.begin() + start_segment_index);
        }

        cur = &string[start_segment_index];

        cur->str.erase(0, end - cur->start);

        if(cur->str == "") {
            if(string.size() > 1) {
                string.erase(string.begin() + start_segment_index);
            }
        }

        UpdateSegmentStart(start_segment_index);
    }
}

void StyledString::Clear(Style style) {
    string.resize(1);
    string[0] = StyledSegment("", style, 0);
}

void StyledString::ClearStyle(Style style) {
    std::string new_str;

    for(StyledSegment segment : string) {
        new_str.append(segment.str);
    }

    string.resize(1);
    string[0] = StyledSegment(new_str, style, 0);
}

void StyledString::UpdateRaw() {
    std::string new_raw("");

    Style state = string[0].style;

    for(StyledSegment segment : string) {
        new_raw.append(segment.style.GetEscapeCode(&state));
        new_raw.append(segment.str);
        state = segment.style;
    }

    raw = new_raw;
}

void StyledString::UpdateSegmentStart(uint32_t i) {
    uint32_t next_start;

    if(i >= string.size()) {
        throw std::runtime_error("sus");
    }

    if(i == 0) {
        next_start = 0;
    } else {
        next_start = string[i].start;
    }

    while(i < string.size()) {
        string[i].start = next_start;
        next_start += string[i].str.length();
        i++;
    }
}

void StyledString::BoundsCheck(uint32_t index, std::string message) {
    if(index > Len()) {
        throw std::runtime_error(message);
    }
}

uint32_t StyledString::GetSegmentIndex(uint32_t index) {
    uint32_t segment_index = 0;

    if(index >= string[string.size() - 1].start) {
        return string.size() - 1;
    }

    for(uint32_t i = 0; i < string.size() - 1; i++) {
        if(index < string[i + 1].start) {
            segment_index = i;

            if(string[i + 1].start == string[i].start) {
                continue;
            } else {
                break;
            }
        }
    }

    return segment_index;
}

uint32_t StyledString::Len() {
    StyledSegment& last = string.back();
    return last.start + last.str.length() + 1;
}

void StyledString::Resize(uint32_t size) {
    if(size < Len()) {
        uint32_t start_segment_index = GetSegmentIndex(size);

        uint32_t len = string.size();
        for(uint32_t i = start_segment_index + 1; i < len; i++) {
            string.erase(string.begin() + start_segment_index + 1);
        }

        string[start_segment_index].Split(size - string[start_segment_index].start);
    } else {
        string[string.size() - 1].str.resize(size - string[string.size() - 1].start, ' ');
    }
}

std::string& StyledString::Raw(Style* state) {
    raw = string[0].style.GetEscapeCode(state) + raw;
    return raw;
}

Style StyledString::StyleStart() {
    return string[0].style;
}

Style StyledString::StyleEnd() {
    return string[string.size() - 1].style;
}

void StyledString::Print(Style* state) {
    printf(Raw(state).c_str());
    StyleEnd().SetState(state);
}

StyledString StyledString::Substr(uint32_t start, uint32_t end) {
    if(start >= Len()) throw std::runtime_error("Index of start " + std::to_string(start) + " is out of bounds! << StyledString::Substr()");
    if(end >= Len()) throw std::runtime_error("Index of end " + std::to_string(end) + " is out of bounds! << StyledString::Substr()");

    uint32_t start_segment_index = GetSegmentIndex(start);
    uint32_t end_segment_index = GetSegmentIndex(end);

    std::vector<StyledSegment> substr;

    if(start_segment_index == end_segment_index) {
        StyledSegment segment = string[start_segment_index];
        substr.push_back(StyledSegment(segment.str.substr(start - segment.start, end - segment.start), segment.style, 0));
    } else {
        StyledSegment start_segment = string[start_segment_index];

        substr.push_back(StyledSegment(start_segment.str.substr(start - start_segment.start), start_segment.style, 0));

        for(uint32_t i = start_segment_index + 1; i < end_segment_index; i++) {
            substr.push_back(string[i]);
        }

        StyledSegment end_segment = string[end_segment_index];
        substr.push_back(StyledSegment(end_segment.str.substr(0, end - end_segment.start), end_segment.style, substr.back().start + 1));
    }

    return StyledString(substr);
}

} // namespace LibTesix