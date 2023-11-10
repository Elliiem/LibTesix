#pragma once
#include "Window.h"

#include <termios.h>

static const int STDIN = 0;

namespace LibTesix {

void Interupt_Handler(int signal);

class Screen {
  public:
    Screen();
    ~Screen();

    std::vector<Window> windows;

    Style state;

    void Clear(Style style);
    void ClearBox(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Style style);

    void Update();
};

} // namespace LibTesix