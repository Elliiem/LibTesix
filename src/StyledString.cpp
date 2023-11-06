#include "StyledString.h"

#include "SegmentArray.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

namespace LibTesix {

StyledSegment::StyledSegment(const icu::UnicodeString& str, Style style, uint32_t start) {
    this->str = icu::UnicodeString(str);
    this->style = style;
    this->start = start;
}

StyledSegment::StyledSegment(const char* str, Style style, uint32_t start) {
    this->str = icu::UnicodeString(str);
    this->style = style;
    this->start = start;
}

StyledSegment::StyledSegment() {
    str = icu::UnicodeString("");

    style = STANDARD_STYLE;
    start = 0;
}

StyledSegment StyledSegment::Split(uint32_t index) {
    if(index > str.length()) throw std::runtime_error("Index " + std::to_string(index) + " is out of bounds! << StyledSegment::Split()");

    icu::UnicodeString substr(str, index);

    StyledSegment new_seg(substr, style, start + index);

    str.remove(index);

    return new_seg;
}

SSA::SSA(const std::vector<StyledSegment>& string) {
    this->string = std::vector<StyledSegment>(string);
}

SSA::SSA() {
}

uint32_t SSA::GetSegmentIndex(uint32_t index) {
    uint32_t segment_index = 0;

    if(string.size() == 0) {
        return 0;
    }

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

StyledString::StyledString(const icu::UnicodeString& base_string, Style style) {
    segments.string.push_back(StyledSegment(base_string, style));
    UpdateRaw();
}

StyledString::StyledString(const char* base_string, Style style) {
    segments.string.push_back(StyledSegment(base_string, style));
}

StyledString::StyledString(const SSA& segments) {
    if(segments.string.size() == 0) {
        this->segments.string.clear();
        this->segments.string.push_back(StyledSegment());
    } else {
        this->segments = SSA(segments);
    }

    UpdateRaw();
}

StyledString::StyledString(const std::vector<StyledSegment>& string) {
    if(string.size() == 0) {
        this->segments.string.clear();
        this->segments.string.push_back(StyledSegment());
    } else {
        this->segments.string = std::vector<StyledSegment>(string);
    }

    UpdateRaw();
}

StyledString::StyledString() {
    segments.string.push_back(StyledSegment());
}

void StyledString::Insert(const icu::UnicodeString& str, Style style, uint32_t index) {
    uint32_t start_segment_index = segments.GetSegmentIndex(index);
    StyledSegment seg = segments.string[start_segment_index].Split(index - segments.string[start_segment_index].start);

    segments.string.insert(segments.string.begin() + start_segment_index + 1, seg);

    segments.string.insert(segments.string.begin() + start_segment_index + 1, StyledSegment(str, style, index));

    UpdateSegmentStart(start_segment_index + 1);
}

void StyledString::Insert(const char* str, Style style, uint32_t index) {
    icu::UnicodeString uc_str(str);

    Insert(uc_str, style, index);
}

void StyledString::Append(const icu::UnicodeString& str, Style style) {
    if(segments.string.size() == 1 && segments.string[0].str == "") {
        segments.string[0] = StyledSegment(str, style, Len());
    } else {
        segments.string.push_back(StyledSegment(str, style, Len()));
    }
}

void StyledString::Append(const char* str, Style style) {
    icu::UnicodeString uc_str(str);

    Append(uc_str, style);
}

void StyledString::Erase(uint32_t start, uint32_t end) {
    BoundsCheck(start, "Index of start " + std::to_string(start) + " is out of bounds! << StyledString::Erase()");
    BoundsCheck(end, "Index of end " + std::to_string(end) + " is out of bounds! << StyledString::Erase()");

    uint32_t start_segment_index = segments.GetSegmentIndex(start);
    uint32_t end_segment_index = segments.GetSegmentIndex(end);

    StyledSegment* cur = &segments.string[start_segment_index];

    if(start_segment_index == end_segment_index) {
        uint32_t erase_start = start - cur->start;
        uint32_t erase_len = end - cur->start - erase_start;

        cur->str.remove(erase_start, erase_len);

        segments.string.insert(segments.string.begin() + start_segment_index + 1, cur->Split(erase_start));

        if(cur->str.isEmpty() && segments.string.size() > 1) {
            segments.string.erase(segments.string.begin() + start_segment_index);
        }
    } else {
        cur->Split(start - cur->start);
        if(cur->str.isEmpty()) {
            segments.string.erase(segments.string.begin() + start_segment_index);
            end_segment_index--;
            start_segment_index--;
        }

        for(uint32_t i = start_segment_index + 1; i < end_segment_index; i++) {
            segments.string.erase(segments.string.begin() + start_segment_index + 1);
        }

        cur = &segments.string[start_segment_index + 1];

        *cur = cur->Split(end - cur->start);

        if(cur->str.isEmpty()) {
            if(segments.string.size() > 1) {
                segments.string.erase(segments.string.begin() + start_segment_index);
            }
        }
    }

    UpdateSegmentStart(start_segment_index);
}

icu::UnicodeString StyledString::Write(const icu::UnicodeString& str, Style style, uint32_t index) {
    BoundsCheck(index + 1, "Index " + std::to_string(index) + " is out of bounds! << StyledString::Write()");

    icu::UnicodeString overflow_str;
    icu::UnicodeString uc_str(str);

    int32_t over = str.length() + index - Len();

    if(over > 0) {
        str.extractBetween(str.length() - over, str.length(), overflow_str);
        uc_str.truncate(uc_str.length() - over);
    }

    Erase(index, index + uc_str.length());

    uint32_t write_segment_index = segments.GetSegmentIndex(index);

    StyledSegment write_segment(uc_str, style, 0);

    bool back = !(segments.string[write_segment_index].start == index);
    if(segments.string.size() > 1) {
        segments.string.insert(segments.string.begin() + write_segment_index + back, write_segment);
    } else {
        if(back) {
            segments.string.push_back(write_segment);
        } else {
            segments.string.insert(segments.string.begin(), write_segment);
        }
    }

    UpdateSegmentStart(write_segment_index);

    return overflow_str;
}

icu::UnicodeString StyledString::Write(const char* str, Style style, uint32_t index) {
    icu::UnicodeString uc_str(str);

    return Write(uc_str, style, index);
}

StyledString StyledString::Substr(uint32_t start, uint32_t end) {
    if(end < start) std::swap(end, start);

    BoundsCheck(start, "Index of start " + std::to_string(start) + " is out of bounds! << StyledString::Substr()");
    BoundsCheck(end, "Index of end " + std::to_string(end) + " is out of bounds! << StyledString::Substr()");

    uint32_t start_segment_index = segments.GetSegmentIndex(start);
    uint32_t end_segment_index = segments.GetSegmentIndex(end);

    std::vector<StyledSegment> substr;

    if(start_segment_index == end_segment_index) {
        StyledSegment segment = segments.string[start_segment_index];

        icu::UnicodeString uc_substr(segment.str, start - segment.start, (end - segment.start) - (start - segment.start));
        if(!uc_substr.isEmpty()) substr.push_back(StyledSegment(uc_substr, segment.style, 0));
    } else {
        StyledSegment start_segment = segments.string[start_segment_index];

        icu::UnicodeString uc_substr(start_segment.str, start - start_segment.start);
        if(!uc_substr.isEmpty()) substr.push_back(StyledSegment(uc_substr, start_segment.style, 0));

        for(uint32_t i = start_segment_index + 1; i < end_segment_index; i++) {
            substr.push_back(segments.string[i]);
        }

        StyledSegment end_segment = segments.string[end_segment_index];

        uc_substr = icu::UnicodeString(end_segment.str, 0, end - end_segment.start);
        if(!uc_substr.isEmpty()) substr.push_back(StyledSegment(uc_substr, end_segment.style, substr.back().start + substr.back().str.length()));
    }

    return StyledString(substr);
}

uint32_t StyledString::Len() {
    StyledSegment& last = segments.string.back();
    return last.start + last.str.length();
}

void StyledString::Resize(uint32_t size) {
    if(size < Len()) {
        uint32_t start_segment_index = segments.GetSegmentIndex(size);

        uint32_t len = segments.string.size();
        for(uint32_t i = start_segment_index + 1; i < len; i++) {
            segments.string.erase(segments.string.begin() + start_segment_index + 1);
        }

        segments.string[start_segment_index].Split(size - segments.string[start_segment_index].start);
    } else {
        std::string right_pad(size - Len(), ' ');
        segments.string.back().str.append(icu::UnicodeString(right_pad.c_str()));
    }
}

void StyledString::Clear(Style style) {
    segments.string.resize(1);
    segments.string[0] = StyledSegment("", style, 0);
}

void StyledString::ClearStyle(Style style) {
    icu::UnicodeString seg_str;

    for(StyledSegment segment : segments.string) {
        seg_str.append(segment.str);
    }

    segments.string.clear();
    segments.string.push_back(StyledSegment(seg_str, style, 0));
}

void StyledString::UpdateRaw() {
    std::string new_raw("");

    Style state = segments.string[0].style;

    std::string uc_str;

    for(StyledSegment segment : segments.string) {
        new_raw.append(segment.style.GetEscapeCode(state));

        uc_str.clear();
        segment.str.toUTF8String(uc_str);
        new_raw.append(uc_str);

        state = segment.style;
    }

    raw = new_raw;
}

const std::string& StyledString::Raw(const Style& state, bool should_update) {
    if(should_update) UpdateRaw();
    raw = segments.string[0].style.GetEscapeCode(state) + raw;
    return raw;
}

Style StyledString::StyleStart() {
    return segments.string[0].style;
}

Style StyledString::StyleEnd() {
    return segments.string.back().style;
}

void StyledString::Print(Style& state, bool should_update) {
    printf(Raw(state, should_update).c_str());
    state = StyleEnd();
}

void StyledString::UpdateSegmentStart(uint32_t i) {
    uint32_t next_start;

    if(i >= segments.string.size()) {
        throw std::runtime_error("Index " + std::to_string(i) + " is out of bounds! << StyledString::UpdateSegmentStart()");
    }

    if(i == 0) {
        next_start = 0;
    } else {
        next_start = segments.string[i - 1].start + segments.string[i - 1].str.length();
    }

    while(i < segments.string.size()) {
        segments.string[i].start = next_start;
        next_start += segments.string[i].str.length();
        i++;
    }
}

void StyledString::BoundsCheck(uint32_t index, std::string message) {
    if(index > Len()) {
        throw std::runtime_error(message);
    }
}

} // namespace LibTesix