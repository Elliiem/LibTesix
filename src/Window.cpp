#include "Window.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace LibTesix {

Window::Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

    std::string fill(width - 1, ' ');
    icu::UnicodeString styled_str(fill.c_str());
    lines.resize(height, StyledString(styled_str));
}

void Window::Print(uint32_t x, uint32_t y, icu::UnicodeString& str, Style style) {
    if(x >= width) throw std::runtime_error("x:" + std::to_string(x) + "is out of bounds! << Window::Print()");
    else if(y >= height)
        throw std::runtime_error("y:" + std::to_string(y) + "is out of bounds! << Window::Print()");

    icu::UnicodeString overflow;

    overflow = lines[y].Write(str, style, x);
    y++;
    while(!overflow.isEmpty() && y < height) {
        overflow = lines[y].Write(overflow, style, 0);
        y++;
    }
}

void Window::UpdateRaw() {
    if(lines.size() == 0) {
        raw = "";
        return;
    }

    std::string new_raw;

    interval x_visible = GetXVisible();
    interval y_visible = GetYVisible();

    if((x_visible.first == 0 && x_visible.second == 0) || (y_visible.first == 0 && y_visible.second == 0)) {
        raw = "";
        return;
    }

    Style state = lines[y_visible.first].StyleStart();
    raw_start_style = state;

    uint32_t clipped_x;
    clipped_x = x * !(x < 0);

    for(uint32_t i = y_visible.first; i <= y_visible.second; i++) {
        new_raw.append("\033[" + std::to_string(y + i + 1) + ";" + std::to_string(clipped_x + 1) + "f");
        lines[i].UpdateRaw();
        StyledString visible = lines[i].Substr(x_visible.first, x_visible.second);
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

Window::interval Window::GetXVisible() {
    uint32_t term_width = GetTerminalWidth();

    interval ret(0, 0);

    if(x < 0) {
        if(x + (int32_t)width <= 0) {
            return ret;
        }

        ret.first = abs(x);

        if(x + width > term_width) {
            ret.second = width - (x + width - term_width);
        } else {
            ret.second = width - 1;
        }

        return ret;
    } else if(x + width > term_width) {
        if(x >= term_width) {
            return ret;
        }

        ret.second = width - (x + width - term_width);
        return ret;
    } else {
        ret.second = width - 1;

        return ret;
    }
}

Window::interval Window::GetYVisible() {
    uint32_t term_height = GetTerminalHeight();

    interval ret(0, 0);

    if(y <= 0) {
        if(y + (int32_t)height <= 0) {
            return ret;
        }

        ret.first = abs(y);

        if(y + height > term_height) {
            ret.second = height - (y + height - term_height);
        } else {
            ret.second = height - 1;
        }

        return ret;
    } else if(y + height > term_height) {
        if(y >= term_height) {
            return ret;
        }

        ret.second = height - (y + height - term_height);
        return ret;
    } else {
        ret.second = height - 1;

        return ret;
    }
}

uint32_t GetTerminalWidth() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    return 211;
    return w.ws_col;
}

uint32_t GetTerminalHeight() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    return 49;
    return w.ws_row;
}

} // namespace LibTesix