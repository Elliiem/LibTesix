#pragma once
#include "Screen.h"
#include "Style.h"
#include "StyledString.h"

#include <chrono>
#include <cinttypes>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

namespace LibTesix {

void Dev() {
    /*Screen src;

    Window win(10, 10, 10, 5);

    Style background;
    background.BG(Color(0, 0, 50));

    Style foo;
    foo.Blinking(0)->BG(Color(50, 50, 50))->Italic(1);

    win.Print(0, 0, "############################################┛┛┛┛┛┛", foo);

    printf("%s", background.GetEscapeCode(&src.state).c_str());
    src.state = background;
    printf("\033[2J");

    struct Vel {
        uint32_t x;
        uint32_t y;
    };

    Vel vel;
    vel.x = 2;
    vel.y = 1;
    while(true) {
        win.Draw(&src.state, 1);
        printf("\033[0;0f\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if(win.x >= GetTerminalWidth() - win.width) {
            vel.x = -vel.x;
        } else if(win.x <= 0) {
            vel.x = -vel.x;
        }

        if(win.y >= GetTerminalHeight() - win.height) {
            vel.y = -vel.y;
        } else if(win.y <= 0) {
            vel.y = -vel.y;
        }

        win.x += vel.x;
        win.y += vel.y;

        printf("%s", background.GetEscapeCode(&src.state).c_str());
        src.state = background;
        printf("\033[2J");
    }

    printf("\033[0m\033[2J\n\033[0;0f");*/

    StyledString foo("Susybaka!");
    foo.Dev();
}

} // namespace LibTesix