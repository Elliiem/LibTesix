#include "Overlay.h"

#include "rapidjson/pointer.h"

#include <fstream>
#include <sstream>

namespace LibTesix {
//

Overlay::Overlay() {
}

Overlay::Overlay(uint64_t width, uint64_t height) {
    this->width = width;
    this->height = height;

    lines.resize(height);
}

Overlay::Overlay(JsonDocument& json, const char* name) {
    LoadFromJson(json, name);
}

Overlay::Overlay(rapidjson::Value& json_overlay) {
    LoadFromJson(json_overlay);
}

void Overlay::Clear() {
    for(StyledSegmentArray& arr : lines) {
        arr.Clear();
    }
}

void Overlay::Box(uint64_t x, uint64_t y, uint64_t width, uint64_t height, const Style* style, const char* right, const char* left, const char* top,
    const char* bottom, const char* top_right, const char* top_left, const char* bottom_right, const char* bottom_left) {
    icu::UnicodeString top_str;
    top_str.append(top_right);
    for(uint64_t i = 0; i < width - 2; i++) {
        top_str.append(icu::UnicodeString(top));
    }
    top_str.append(top_left);

    StyledSegmentArray middle;
    middle.Add(right, style, 0);
    middle.Add(left, style, width - 1);

    icu::UnicodeString bottom_str;
    bottom_str.append(bottom_right);
    for(uint64_t i = 0; i < width - 2; i++) {
        bottom_str.append(icu::UnicodeString(bottom));
    }
    bottom_str.append(bottom_left);

    lines[0].Add(top_str, style, 0);

    for(uint64_t i = 1; i < height - 1; i++) {
        lines[i] = middle;
    }

    lines[height - 1].Add(bottom_str, style, 0);
}

void Overlay::Box(const Style* style, const char* right, const char* left, const char* top, const char* bottom, const char* top_right,
    const char* top_left, const char* bottom_right, const char* bottom_left) {
    UpdateWidth();
    Box(0, 0, width, height, style, right, left, top, bottom, top_right, top_left, bottom_right, bottom_left);
}

void Overlay::UpdateWidth() {
    uint64_t new_width {};

    for(StyledSegmentArray& arr : lines) {
        for(std::shared_ptr<StyledSegment> seg : arr.segments) {
            if(seg.get()->start + seg.get()->str.length() > new_width) {
                new_width = seg.get()->start + seg.get()->str.length();
            }
        }
    }
    width = new_width;
}

} // namespace LibTesix