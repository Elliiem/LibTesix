#include "StyledString.h"

#include <stdexcept>

namespace LibTesix {

// TODO update raw in constructors
StyledString::StyledString(const icu::UnicodeString& base_string, const Style* style) {
    Append(base_string, style);
    UpdateRaw();
}

StyledString::StyledString(const char* base_string, const Style* style) {
    Append(base_string, style);
    UpdateRaw();
}

StyledString::StyledString(const StyledSegmentArray& string) {
    segments = string.segments;
    UpdateRaw();
}

StyledString::StyledString(const std::vector<StyledSegment>& string) {
    segments = std::vector<StyledSegment>(string);
    UpdateRaw();
}

StyledString::StyledString() {
    Append("", style_allocator[0UL]);
    UpdateRaw();
}

void StyledString::Insert(const icu::UnicodeString& str, const Style* style, uint64_t index) {
    if(index > Len()) throw std::runtime_error("Index " + std::to_string(index) + " is out of bounds! << StyledString::Insert)");

    uint64_t segment_index = GetSegmentIndex(index);

    if(index == segments[segment_index].start) {
        InsertSegment(StyledSegment(str, style, index), segment_index);
    } else {
        InsertSegment(segments[segment_index].Split(index - segments[segment_index].start), segment_index + 1);
        InsertSegment(StyledSegment(str, style, index), segment_index + 1);
    }

    UpdateSegmentStart(segment_index);
}

void StyledString::Insert(const char* str, const Style* style, uint64_t index) {
    icu::UnicodeString uc_str(str);
    Insert(uc_str, style, index);
}

void StyledString::Append(const icu::UnicodeString& str, const Style* style) {
    if(segments.size() == 1 && Len() == 0) {
        segments[0] = StyledSegment(str, style, 0);
    } else {
        StyledSegmentArray::Append(str, style);
    }
}

void StyledString::Append(const char* str, const Style* style) {
    icu::UnicodeString uc_str(str);
    Append(uc_str, style);
}

void StyledString::Erase(uint64_t start, uint64_t end) {
    uint64_t start_segment_index = GetSegmentIndex(start);

    StyledSegmentArray::Erase(start, end);

    UpdateSegmentStart(start_segment_index);
}

icu::UnicodeString StyledString::Write(const icu::UnicodeString& str, const Style* style, uint64_t index) {
    if(index >= Len()) throw std::runtime_error("Index " + std::to_string(index) + " is out of bounds << StyledString::Write()");

    if(str.length() == 0) return icu::UnicodeString();

    icu::UnicodeString write_string(str);
    icu::UnicodeString overflow;

    int64_t over = (index + str.length()) - Len();

    if(over > 0) {
        uint64_t overflow_index = write_string.length() - over;

        write_string.extractBetween(overflow_index, write_string.length(), overflow);
        write_string.remove(overflow_index);
    }

    Add(write_string, style, index);

    return overflow;
}

icu::UnicodeString StyledString::Write(const char* str, const Style* style, uint64_t index) {
    icu::UnicodeString uc_str(str);
    return Write(uc_str, style, index);
}

StyledString StyledString::Substr(uint64_t start, uint64_t end) {
    std::vector<StyledSegment> substr_segments;

    uint64_t start_segment_index = GetSegmentIndex(start);
    uint64_t end_segment_index = GetSegmentIndex(end);

    if(start_segment_index == end_segment_index) {
        icu::UnicodeString segment_substr;
        uint64_t segment_start = segments[start_segment_index].start;

        segments[start_segment_index].str.extractBetween(start - segment_start, end - segment_start + 1, segment_substr);
        substr_segments.push_back(StyledSegment(segment_substr, segments[start_segment_index].style, 0));
    } else {
        icu::UnicodeString segment_substr;

        // get substring of the start segment
        // ...and add that to substr_segments
        uint64_t start_segment_len = segments[start_segment_index].Len();
        uint64_t start_segment_start = segments[start_segment_index].start;

        segments[start_segment_index].str.extractBetween(start - start_segment_start, start_segment_len, segment_substr);
        substr_segments.push_back(StyledSegment(segment_substr, segments[start_segment_index].style, 0));

        // Add every segment between the start segment and the end segment
        for(uint64_t i = start_segment_index + 1; i < end_segment_index; i++) {
            uint64_t substr_len = substr_segments.back().start + substr_segments.back().Len();
            substr_segments.push_back(StyledSegment(segments[i].str, segments[i].style, substr_len));
        }

        // get substring of the end segment
        // ...and add that to substr_segments
        uint64_t end_segment_start = segments[end_segment_index].start;
        uint64_t substr_len = substr_segments.back().start + substr_segments.back().Len();

        segments[end_segment_index].str.extractBetween(0, end - end_segment_start + 1, segment_substr);
        substr_segments.push_back(StyledSegment(segment_substr, segments[end_segment_index].style, substr_len));
    }

    return StyledString(substr_segments);
}

void StyledString::Resize(uint64_t size) {
    if(size == Len()) return;

    if(size > Len()) {
        segments.back().str.append(std::string(size - Len(), ' ').c_str());
    } else {
        StyledSegmentArray::Erase(size, Len() - 1);
        if(segments.size() == 0) {
            Append("", style_allocator[0UL]);
        }
    }
}

void StyledString::Clear(const Style* style) {
    StyledSegmentArray::Clear();

    Append("", style);
}

void StyledString::ClearStyle(const Style* style) {
    icu::UnicodeString new_seg_str;

    for(StyledSegment segment : segments) {
        new_seg_str.append(segment.str);
    }

    StyledSegmentArray::Clear();

    Append(new_seg_str, style);
}

void StyledString::UpdateRaw() {
    std::string new_raw;

    Style state = *segments[0].style;

    for(StyledSegment segment : segments) {
        new_raw.append(segment.style->GetEscapeCode(state));
        segment.str.toUTF8String(new_raw);

        state = *segment.style;
    }

    raw = new_raw;
}

std::string StyledString::Raw(const Style& state, bool should_update) {
    if(should_update) UpdateRaw();

    return segments[0].style->GetEscapeCode(state) + raw;
}

const Style* StyledString::StyleStart() const {
    return segments[0].style;
}

const Style* StyledString::StyleEnd() const {
    return segments.back().style;
}

void StyledString::Print(Style& state, bool should_update) {
    printf(Raw(state).c_str());
    printf("\n");

    state = *StyleEnd();
}

void StyledString::UpdateSegmentStart(uint64_t index) {
    uint64_t next_start = 0;
    if(index > 0) {
        next_start = segments[index - 1].start + segments[index - 1].Len();
    }

    for(uint64_t i = index; i < segments.size(); i++) {
        segments[i].start = next_start;

        next_start = segments[i].start + segments[i].Len();
    }
}

} // namespace LibTesix