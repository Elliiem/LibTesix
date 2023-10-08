#include "Window.h"

#include <iostream>

namespace LibTesix {

Window::Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    x++;
    this->x = x;
    y++;
    this->y = y;
    this->width = width;
    this->height = height;

    std::string fill;
    fill.resize(width - 1, ' ');
    lines.resize(height, StyledString(fill));
}

void Window::Print(uint32_t x, uint32_t y, std::string str, Style style) {
    if(x >= width) throw std::runtime_error("x:" + std::to_string(x) + "is out of bounds! << Window::Print()");
    else if(y >= height)
        throw std::runtime_error("y:" + std::to_string(y) + "is out of bounds! << Window::Print()");

    str = lines[y].Write(str, style, x);
    y++;
    while(str != "" && y < height) {
        str = lines[y].Write(str, style, 0);
        y++;
    }
}

void Window::Update() {
    if(lines.size() == 0) {
        raw_string = "";
        return;
    }

    std::string new_raw;
    Style state = lines[0].StyleStart();

    for(uint32_t i = 0; i < lines.size(); i++) {
        new_raw.append("\033[" + std::to_string(y + i) + ";" + std::to_string(x) + "f");
        lines[i].UpdateRaw();
        new_raw.append(lines[i].Raw(&state));
        state = lines[i].StyleEnd();
    }

    raw_string = new_raw;
}

void Window::Draw(Style* state, bool should_update) {
    if(should_update) {
        Update();
    }

    if(lines.size() != 0) {
        printf(lines[0].StyleStart().GetEscapeCode(state).c_str());

        printf(raw_string.c_str());

        *state = lines[lines.size() - 1].StyleEnd();
    } else {
        printf(raw_string.c_str());
    }
}

} // namespace LibTesix