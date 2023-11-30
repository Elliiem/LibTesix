#include "Window.h"

#include "Terminal.h"

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

namespace LibTesix {

Window::Window(int64_t x, int64_t y, uint64_t width, uint64_t height, const Style* style) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

    StyledString fill;
    fill.Resize(width);
    fill.ClearStyle(style);

    lines.resize(height, fill);
}

Window::Window(JsonDocument& json, const char* name) {
    LoadFromJson(json, name);
}

Window::Window(rapidjson::Value& json_window) {
    LoadFromJson(json_window);
}

void Window::Write(uint64_t col, uint64_t line, icu::UnicodeString& str, const Style* style) {
    if(col >= width) throw std::runtime_error("x: " + std::to_string(x) + " is out of bounds! << Window::Print()");
    else if(line >= height)
        throw std::runtime_error("y: " + std::to_string(y) + " is out of bounds! << Window::Print()");

    icu::UnicodeString overflow;

    overflow = lines[line].Write(str, style, col);
    line++;
    while(!overflow.isEmpty() && line < height) {
        overflow = lines[line].Write(overflow, style, 0);
        line++;
    }
}

void Window::Write(uint64_t col, uint64_t line, const char* str, const Style* style) {
    icu::UnicodeString uc_str(str);

    Write(col, line, uc_str, style);
}

void Window::UpdateRaw() {
    if(lines.size() == 0) {
        raw = "";
        return;
    }

    std::string new_raw;

    Range x_visible = ClampRange(GetTerminalWidth(), Range(x, x + width));
    Range y_visible = ClampRange(GetTerminalHeight(), Range(y, y + height));

    if((x_visible.first == -1 && x_visible.second == -1) || (y_visible.first == -1 && y_visible.second == -1)) {
        raw = "";
        return;
    }

    Style state = *lines[y_visible.first].StyleStart();
    raw_start_style = lines[y_visible.first].StyleStart();

    uint64_t clipped_x;
    clipped_x = x * (x > 0);

    for(uint64_t i = y_visible.first; i < y_visible.second; i++) {
        new_raw.append("\033[" + std::to_string(y + i + 1) + ";" + std::to_string(clipped_x + 1) + "f");

        StyledString visible = lines[i].Substr(x_visible.first, x_visible.second);
        if(overlay_enabled && i < overlay.height) ApplySegmentArray(overlay.lines[i], visible, x_visible.first);

        new_raw.append(visible.Raw(state, 0));
        state = *visible.StyleEnd();

        raw_end_style = visible.StyleEnd();
    }

    raw = new_raw;
}

void Window::Draw(Style& state, bool should_update) {
    if(should_update) {
        UpdateRaw();
    }

    printf(raw_start_style->GetEscapeCode(state).c_str());
    printf(raw.c_str());

    state = *raw_end_style;
}

uint64_t Window::GetHeight() {
    return height;
}

uint64_t Window::GetWidth() {
    return width;
}

int64_t Window::GetX() {
    return x;
}

int64_t Window::GetY() {
    return y;
}

void Window::Move(int64_t x, int64_t y) {
    this->x = x;
    this->y = y;
}

void Window::Resize(uint64_t width, uint64_t height) {
    lines.resize(height);

    for(StyledString& str : lines) {
        str.Resize(width);
    }

    this->width = width;
    this->height = height;
}

void Window::ApplyOverlay(Overlay& overlay) {
    this->overlay = Overlay(overlay);
    overlay_enabled = true;
    has_overlay = true;
}

void Window::ApplyOverlay() {
    overlay_enabled = true;
}

void Window::RemoveOverlay() {
    overlay_enabled = false;
}

void Window::Clear(const Style* style) {
    for(StyledString& str : lines) {
        str.Clear(style);
        str.Resize(width);
    }
}

Range ClampRange(uint64_t max, Range range) {
    Range ret(-1, -1);

    int64_t range_len = range.second - range.first;

    if(range.first <= 0) {
        if(range.first + range_len <= 0) {
            return ret;
        }

        ret.first = abs(range.first);

        if(range.second > max) {
            ret.second = range_len - (range.second - max) - 1;
        } else {
            ret.second = range_len;
        }

        return ret;
    } else if(range.second > max) {
        if(range.first >= max) {
            return ret;
        }

        ret.first = 0;
        ret.second = range_len - (range.second - max) - 1;
        return ret;
    } else {
        ret.first = 0;
        ret.second = range_len;

        return ret;
    }
}

void ApplySegmentArray(StyledSegmentArray& arr, StyledString& str, uint64_t offset) {
    for(StyledSegment& seg : arr.segments) {
        if(seg.start >= offset + str.Len()) {
            break;
        }

        if(seg.start < offset && seg.start + seg.Len() > offset) {
            icu::UnicodeString segment_substr(seg.str, offset, seg.Len() - offset);
            str.Write(segment_substr, seg.style, 0);
        } else if(seg.start >= offset) {
            str.Write(seg.str, seg.style, seg.start - offset);
        }
    }

    str.UpdateRaw();
}

} // namespace LibTesix