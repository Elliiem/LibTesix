#pragma once
#include "Screen.h"
#include "Style.h"
#include "StyledString.h"

#include <cinttypes>
#include <iostream>
#include <thread>

namespace LibTesix {

void Dev() {
    Screen scr;

    Style foo;
    foo.Blinking(0)->BG(Color(50, 50, 50))->Italic(1);

    Window win(10, 10, 10, 5);

    icu::UnicodeString win_text("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW");
    win.Write(0, 0, win_text, foo);

    Style background;
    background.BG(Color(0, 0, 50));

    scr.Clear(background);

    int32_t x_vel = 2;
    int32_t y_vel = 1;
    while(true) {
        win.Draw(scr.state, 1);

        scr.Update();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        if(win.GetX() >= GetTerminalWidth() - win.GetWidth() || win.GetX() <= 0) {
            x_vel = -x_vel;
        }

        if(win.GetY() >= GetTerminalHeight() - win.GetHeight() || win.GetY() <= 0) {
            y_vel = -y_vel;
        }

        win.Move(win.GetX() + x_vel, win.GetY() + y_vel);

        scr.Clear(background);
    }
}

} // namespace LibTesix

// 🮲🮳