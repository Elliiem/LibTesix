#pragma once

#include "Style.h"

#include <termios.h>

// Overrides the size of the terminal to 211 colums and 41 lines
// #define TTY_SIZE_OVERRIDE

static const int STDIN = 0;

namespace LibTesix {

inline Style state(ColorPair(Color(-1, -1, -1), Color(-1, -1, -1)));

int InitScreen();

void Interupt(int signal);
void Exit();

void Clear(const Style* style);
void Update();

uint64_t GetTerminalWidth();
uint64_t GetTerminalHeight();

} // namespace LibTesix