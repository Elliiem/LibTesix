#include "Window.h"

#include "Terminal.h"

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

namespace LibTesix {

Window::Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Style style) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

    StyledString fill;
    fill.Resize(width);
    fill.ClearStyle(style);

    lines.resize(height, fill);
}

void Window::Write(uint32_t x, uint32_t y, icu::UnicodeString& str, Style style) {
    if(x >= width) throw std::runtime_error("x: " + std::to_string(x) + " is out of bounds! << Window::Print()");
    else if(y >= height)
        throw std::runtime_error("y: " + std::to_string(y) + " is out of bounds! << Window::Print()");

    icu::UnicodeString overflow;

    overflow = lines[y].Write(str, style, x);
    y++;
    while(!overflow.isEmpty() && y < height) {
        overflow = lines[y].Write(overflow, style, 0);
        y++;
    }
}

void Window::Write(uint32_t x, uint32_t y, const char* str, Style style) {
    icu::UnicodeString uc_str(str);

    Write(x, y, uc_str, style);
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

    Style state = lines[y_visible.first].StyleStart();
    raw_start_style = state;

    uint32_t clipped_x;
    clipped_x = x * (x > 0);

    for(uint32_t i = y_visible.first; i < y_visible.second; i++) {
        new_raw.append("\033[" + std::to_string(y + i + 1) + ";" + std::to_string(clipped_x + 1) + "f");

        StyledString visible = lines[i].Substr(x_visible.first, x_visible.second);
        if(overlay_enabled && i < overlay.height) ApplySegmentArray(overlay.lines[i], visible, x_visible.first);

        new_raw.append(visible.Raw(state, 0));
        state = visible.StyleEnd();
    }

    raw_end_style = state;
    raw = new_raw;
}

void Window::Draw(Style& state, bool should_update) {
    if(should_update) {
        UpdateRaw();
    }

    printf(raw_start_style.GetEscapeCode(state).c_str());
    printf(raw.c_str());

    state = raw_end_style;
}

uint32_t Window::GetHeight() {
    return height;
}

uint32_t Window::GetWidth() {
    return width;
}

int32_t Window::GetX() {
    return x;
}

int32_t Window::GetY() {
    return y;
}

void Window::Move(int32_t x, int32_t y) {
    this->x = x;
    this->y = y;
    UpdateRaw();
}

void Window::Resize(int32_t width, int32_t height) {
    lines.resize(height);

    Style state;

    for(StyledString& str : lines) {
        str.Resize(width);
    }

    this->width = width;
    this->height = height;

    int x = 0;
}

void Window::ApplyOverlay(Overlay& overlay) {
    this->overlay = Overlay(overlay);
    overlay_enabled = true;
}

void Window::ApplyOverlay() {
    overlay_enabled = true;
}

void Window::RemoveOverlay() {
    overlay_enabled = false;
}

void Window::Clear(Style style) {
    for(StyledString& str : lines) {
        str.Clear(style);
        str.Resize(width);
    }
}

Range ClampRange(uint32_t max, Range range) {
    Range ret(-1, -1);

    int32_t range_len = range.second - range.first;

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

void ApplySegmentArray(StyledSegmentArray& arr, StyledString& str, uint32_t offset) {
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