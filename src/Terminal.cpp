#include "Terminal.hpp"

#include <csignal>
#include <stdexcept>
#include <sys/ioctl.h>

termios attr;
namespace LibTesix {

int InitScreen() {
    std::signal(SIGINT, Interupt);
    std::atexit(Exit);

    tcgetattr(STDIN, &attr);

    termios new_attr = attr;
    new_attr.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN, TCSANOW, &new_attr);

    // setbuf(stdin, NULL);

    system("clear");
    printf("\033[2J\033[0;0f\033[38;2;255;255;255m\033[48;2;0;0;0m\n");

    return 0;
}

void Interupt(int signal) {
    printf("\033[0m\033[2J\n\033[0;0f");
    tcsetattr(STDIN, TCSANOW, &attr);
    exit(signal);
}

void Exit() {
    printf("\033[0m\033[2J\n\033[0;0f");
    tcsetattr(STDIN, TCSANOW, &attr);
}

void Update() {
    printf("\033[0;0f\n");
}

uint64_t GetTerminalWidth() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

#ifdef TTY_SIZE_OVERRIDE
    return 211;
#else
    return w.ws_col;
#endif
}

uint64_t GetTerminalHeight() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

#ifdef TTY_SIZE_OVERRIDE
    return 49;
#else
    return w.ws_row;
#endif
}

} // namespace LibTesix