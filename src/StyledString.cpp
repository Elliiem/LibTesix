#include "StyledString.h"

#include <iostream>
#include <stdexcept>

namespace LibTesix {

StyledString::StyledSegment::StyledSegment(icu::UnicodeString str, Style style, uint32_t start) {
    this->str = str;
    this->style = style;
    this->start = start;
}

StyledString::StyledSegment::StyledSegment() {
    str = icu::UnicodeString("");

    style = STANDARD_STYLE;
    start = 0;
}

StyledString::StyledSegment StyledString::StyledSegment::Split(uint32_t index) {
    if(index >= str.length()) {
        return *this;
    }

    icu::UnicodeString substr(str, index);

    StyledSegment new_seg(substr, style, start + index);

    str.remove(index);

    return new_seg;
}

StyledString::StyledString(icu::UnicodeString base_string, Style style) {
    string.push_back(StyledSegment(base_string, style));
    UpdateRaw();
}

StyledString::StyledString(std::vector<StyledSegment> string) {
    if(string.size() == 0) {
        this->string.clear();
        this->string.push_back(StyledSegment("", STANDARD_STYLE));
    } else {
        this->string = string;
    }

    UpdateRaw();
}

// TODO Finsish Testing
void StyledString::Insert(std::string str, Style style, uint32_t index) {
    uint32_t start_segment_index = GetSegmentIndex(index);
    StyledSegment seg = string[start_segment_index].Split(index - string[start_segment_index].start);

    string.insert(string.begin() + start_segment_index + 1, seg);

    string.insert(string.begin() + start_segment_index + 1, StyledSegment(icu::UnicodeString(str.c_str()), style, index));

    UpdateSegmentStart(start_segment_index + 1);
}

// TODO Finish Testing
std::string StyledString::Write(std::string str, Style style, uint32_t index) {
    BoundsCheck(index + 1, "Index " + std::to_string(index) + " is out of bounds! << StyledString::Write()");

    icu::UnicodeString uc_str(str.c_str());
    icu::UnicodeString overflow_str;

    int32_t over = uc_str.length() + index - Len();

    if(over > 0) {
        uc_str.extractBetween(uc_str.length() - over, uc_str.length(), overflow_str);
        uc_str.truncate(uc_str.length() - over);
    }

    Erase(index, index + uc_str.length());

    uint32_t start_segment_index = GetSegmentIndex(index);

    StyledSegment seg(icu::UnicodeString(uc_str), style, string[start_segment_index].start + string[start_segment_index].str.length());

    if(string.size() > 1) {
        string.insert(string.begin() + start_segment_index, seg);
    } else {
        string.insert(string.begin(), seg);
    }

    UpdateSegmentStart(start_segment_index);

    std::string ret;
    overflow_str.toUTF8String(ret);

    return ret;
}

void StyledString::Append(std::string str, Style style) {
    if(string.size() == 1 && string[0].str == "") {
        string[0] = StyledSegment(icu::UnicodeString(str.c_str()), style, Len());
    } else {
        string.push_back(StyledSegment(icu::UnicodeString(str.c_str()), style, Len()));
    }
}

// TODO Finish Testing
void StyledString::Erase(uint32_t start, uint32_t end) {
    BoundsCheck(start, "Index of start " + std::to_string(start) + " is out of bounds! << StyledString::Erase()");
    BoundsCheck(end, "Index of end " + std::to_string(end) + " is out of bounds! << StyledString::Erase()");

    uint32_t start_segment_index = GetSegmentIndex(start);
    uint32_t end_segment_index = GetSegmentIndex(end);

    StyledSegment* cur = &string[start_segment_index];

    if(start_segment_index == end_segment_index) {
        uint32_t erase_start = start - cur->start;
        uint32_t erase_len = end - cur->start - erase_start;

        cur->str.remove(erase_start, erase_len);

        if(cur->str.isEmpty() && string.size() > 1) {
            string.erase(string.begin() + start_segment_index);
        }
    } else {
        cur->Split(start - cur->start);
        if(cur->str.isEmpty()) {
            string.erase(string.begin() + start_segment_index);
            end_segment_index--;
            start_segment_index--;
        }

        for(uint32_t i = start_segment_index + 1; i < end_segment_index; i++) {
            string.erase(string.begin() + start_segment_index + 1);
        }

        cur = &string[start_segment_index + 1];

        *cur = cur->Split(end - cur->start);

        if(cur->str.isEmpty()) {
            if(string.size() > 1) {
                string.erase(string.begin() + start_segment_index);
            }
        }
    }

    UpdateSegmentStart(start_segment_index);
}

void StyledString::Clear(Style style) {
    string.resize(1);
    string[0] = StyledSegment("", style, 0);
}

// TODO Finish Testing
void StyledString::ClearStyle(Style style) {
    icu::UnicodeString seg_str;

    for(StyledSegment segment : string) {
        seg_str.append(segment.str);
    }

    string.clear();
    string.push_back(StyledSegment(seg_str, style, 0));
}

// TODO Finish Testing
void StyledString::UpdateRaw() {
    std::string new_raw("");

    Style state = string[0].style;

    std::string uc_str;

    for(StyledSegment segment : string) {
        new_raw.append(segment.style.GetEscapeCode(state));

        uc_str.clear();
        segment.str.toUTF8String(uc_str);
        new_raw.append(uc_str);

        state = segment.style;
    }

    raw = new_raw;
}

// TODO Finish Testing
void StyledString::UpdateSegmentStart(uint32_t i) {
    uint32_t next_start;

    if(i >= string.size()) {
        throw std::runtime_error("Index " + std::to_string(i) + " is out of bounds! << StyledString::UpdateSegmentStart()");
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

    if(index >= string.back().start) {
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

// TODO Finish Testing
uint32_t StyledString::Len() {
    StyledSegment& last = string.back();
    return last.start + last.str.length();
}

// TODO Finish Testing
void StyledString::Resize(uint32_t size) {
    if(size < Len()) {
        uint32_t start_segment_index = GetSegmentIndex(size);

        uint32_t len = string.size();
        for(uint32_t i = start_segment_index + 1; i < len; i++) {
            string.erase(string.begin() + start_segment_index + 1);
            PrintDebug();
        }

        string[start_segment_index].Split(size - string[start_segment_index].start);
    } else {
        std::string right_pad(size - Len(), ' ');
        string.back().str.append(icu::UnicodeString(right_pad.c_str()));
    }
}

std::string& StyledString::Raw(Style& state) {
    raw = string[0].style.GetEscapeCode(state) + raw;
    return raw;
}

Style StyledString::StyleStart() {
    return string[0].style;
}

Style StyledString::StyleEnd() {
    return string.back().style;
}

void StyledString::Print(Style& state) {
    printf(Raw(state).c_str());
    StyleEnd().SetState(state);
}

void StyledString::PrintDebug() {
    std::string uc_str;
    for(StyledSegment seg : string) {
        uc_str.clear();
        seg.str.toUTF8String(uc_str);
        printf("%s, %i | ", uc_str.c_str(), seg.start);
    }
    printf("\n");
}

void StyledString::Dev() {
    Style state;

    Style foo;
    foo.BG(Color(255, 0, 0));
    Write("┥┥┥┥┥┥┥", foo, 0);

    UpdateRaw();
    Print(state);

    printf("\033[0m\n");
}

// TODO Finish testing
StyledString StyledString::Substr(uint32_t start, uint32_t end) {
    if(end < start) std::swap(end, start);

    BoundsCheck(start, "Index of start " + std::to_string(start) + " is out of bounds! << StyledString::Substr()");
    BoundsCheck(end, "Index of end " + std::to_string(end) + " is out of bounds! << StyledString::Substr()");

    uint32_t start_segment_index = GetSegmentIndex(start);
    uint32_t end_segment_index = GetSegmentIndex(end);

    std::vector<StyledSegment> substr;

    if(start_segment_index == end_segment_index) {
        StyledSegment segment = string[start_segment_index];

        icu::UnicodeString uc_substr(segment.str, start - segment.start, (end - segment.start) - (start - segment.start));
        if(!uc_substr.isEmpty()) substr.push_back(StyledSegment(uc_substr, segment.style, 0));
    } else {
        StyledSegment start_segment = string[start_segment_index];

        icu::UnicodeString uc_substr(start_segment.str, start - start_segment.start);
        if(!uc_substr.isEmpty()) substr.push_back(StyledSegment(uc_substr, start_segment.style, 0));

        for(uint32_t i = start_segment_index + 1; i < end_segment_index; i++) {
            substr.push_back(string[i]);
        }

        StyledSegment end_segment = string[end_segment_index];

        uc_substr = icu::UnicodeString(end_segment.str, 0, end - end_segment.start);
        if(!uc_substr.isEmpty()) substr.push_back(StyledSegment(uc_substr, end_segment.style, substr.back().start + substr.back().str.length()));
    }

    return StyledString(substr);
}

} // namespace LibTesix
