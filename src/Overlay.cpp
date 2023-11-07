#include "Overlay.h"

namespace LibTesix {
//

Overlay::Overlay() {
}

Overlay::Overlay(uint32_t width, uint32_t height) {
    this->width = width;
    this->height = height;

    lines.resize(height);
}

void Overlay::Clear() {
    for(StyledSegmentArray& arr : lines) {
        arr.Clear();
    }
}

void Overlay::Box(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char* left, const char* right, const char* top, const char* bottom,
    const char* top_right, const char* top_left, const char* bottom_right, const char* bottom_left) {
    icu::UnicodeString top_str;
    top_str.append(top_right);
    for(uint32_t i = 0; i < width - 2; i++) {
        top_str.append(icu::UnicodeString(top));
    }
    top_str.append(top_left);

    StyledSegmentArray middle;
    middle.Add(right, STANDARD_STYLE, 0);
    middle.Add(left, STANDARD_STYLE, width - 1);

    icu::UnicodeString bottom_str;
    bottom_str.append(bottom_right);
    for(uint32_t i = 0; i < width - 2; i++) {
        bottom_str.append(icu::UnicodeString(bottom));
    }
    bottom_str.append(bottom_left);

    lines[0].Add(top_str, STANDARD_STYLE, 0);

    for(uint32_t i = 1; i < height - 1; i++) {
        lines[i] = middle;
    }

    lines[height - 1].Add(bottom_str, STANDARD_STYLE, 0);
}

} // namespace LibTesix