#pragma once

#include "Style.hpp"

#include <termios.h>

// Overrides the size of the terminal to 211 colums and 41 lines
// #define TTY_SIZE_OVERRIDE

static const int STDIN = 0;

namespace LibTesix {

int InitScreen();

void Interupt(int signal);
void Exit();

void Update();

uint64_t GetTerminalWidth();
uint64_t GetTerminalHeight();

struct TerminalState {
    TerminalState();

    uint64_t height = 0;
    uint64_t width  = 0;
};

} // namespace LibTesix