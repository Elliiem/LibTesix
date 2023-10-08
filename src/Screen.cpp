#include "Screen.h"

namespace LibTesix {

Screen::Screen() {
    tcgetattr(STDIN, &old_attr);
    new_attr = old_attr;
    new_attr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN, TCSANOW, &new_attr);
    setbuf(stdin, NULL);
    printf("\033[2J\033[0;0H\033[38;2;255;255;255m\033[48;2;0;0;0m");
}

Screen::~Screen() {
    tcsetattr(STDIN, TCSANOW, &old_attr);
}

} // namespace LibTesix