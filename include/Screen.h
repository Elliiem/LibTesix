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

  private:
    termios old_attr, new_attr;
};

} // namespace LibTesix