#pragma once

#include "Style.h"

#include <termios.h>

// Overrides the size of the terminal to 211 colums and 41 lines
// #define TTY_SIZE_OVERRIDE

static const int STDIN = 0;

namespace LibTesix {

inline Style state;

int InitScreen();

void Interupt(int signal);
void Exit();

void Clear(Style style);
void Update();

uint32_t GetTerminalWidth();
uint32_t GetTerminalHeight();

} // namespace LibTesix