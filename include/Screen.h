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
    void Update();
};

} // namespace LibTesix