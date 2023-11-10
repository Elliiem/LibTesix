#include "Screen.h"

#include <csignal>
#include <stdexcept>

termios old_attr;
namespace LibTesix {

Screen::Screen() {
    std::signal(SIGINT, Interupt_Handler);

    tcgetattr(STDIN, &old_attr);

    termios new_attr = old_attr;
    new_attr.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN, TCSANOW, &new_attr);

    setbuf(stdin, NULL);

    system("clear");
    printf("\033[2J\033[0;0f\033[38;2;255;255;255m\033[48;2;0;0;0m\n");
}

Screen::~Screen() {
    printf("\033[0m\033[2J\n\033[0;0f");
    tcsetattr(STDIN, TCSANOW, &old_attr);
}

void Screen::Clear(Style style) {
    printf(style.GetEscapeCode(state).c_str());
    state = style;
    printf("\033[2J\033[0;0f\n");
}

void Screen::ClearBox(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Style style) {
    throw std::runtime_error("Not implemented");
}

void Screen::Update() {
    printf("\033[0;0f\n");
}

void Interupt_Handler(int signal) {
    printf("\033[0m\033[2J\n\033[0;0f");
    tcsetattr(STDIN, TCSANOW, &old_attr);
    exit(signal);
}

} // namespace LibTesix