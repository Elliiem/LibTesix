#include "Overlay.h"

#include <stdexcept>

namespace LibTesix {

Overlay::Overlay(uint32_t width, uint32_t height) {
    width = width;

    lines.resize(height);
}

void Overlay::PrintDebug() {
    for(SSA line : lines) {
        for(StyledSegment seg : line.string) {
            std::string utf8;
            seg.str.toUTF8String(utf8);
            printf("%s, %i|", utf8.c_str(), seg.start);
        }
        printf("|\n");
    }
}

void Overlay::Write(uint32_t x, uint32_t y, const icu::UnicodeString& str, Style style) {
}

bool Overlay::HitsSegment(uint32_t x, uint32_t y, uint32_t len) {
    if(y > lines.size()) throw std::runtime_error("Index " + std::to_string(y) + " is out of bounds! << Overlay::HitsSegment()");

    uint32_t start_segment_index = lines[y].GetSegmentIndex(x);
    uint32_t end_segment_index = lines[y].GetSegmentIndex(x + len - 1);

    bool start;
    start = lines[y].string[start_segment_index].start + lines[y].string[start_segment_index].str.length() > x &&
            !(x <= lines[y].string[start_segment_index].start);

    bool end;
    end = lines[y].string[end_segment_index].start + lines[y].string[end_segment_index].str.length() > x + len &&
          !(x + len <= lines[y].string[start_segment_index].start);

    return start || end;
}

bool Overlay::InSegment(uint32_t segment_index, uint32_t index) {
    return false;
}

} // namespace LibTesix