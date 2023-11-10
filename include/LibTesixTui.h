#pragma once

#include "Overlay.h"
#include "Screen.h"
#include "SegmentArray.h"
#include "Style.h"
#include "StyledString.h"

#include <cinttypes>
#include <iostream>
#include <thread>

namespace LibTesix {

void Dev() {
    Screen scr;

    Style foo;
    foo.Blinking(false)->BG(Color(50, 150, 50))->Bold(true);

    Window win(10, 30, 10, 5, foo);

    win.Write(0, 0, "01234567890123456789012345678901234567890123456789", foo);

    foo.Blinking(false);

    Overlay overlay(10, 5);
    overlay.Box(foo);
    win.ApplyOverlay(overlay);

    win.UpdateRaw();

    scr.Clear(foo);

    int32_t x_vel = 2;
    int32_t y_vel = 1;

    while(true) {
        win.Draw(scr.state, 0);

        scr.Update();

        if(win.GetX() + 1 >= GetTerminalWidth() - win.GetWidth() || win.GetX() <= 0) {
            x_vel = -x_vel;
        }

        if(win.GetY() >= GetTerminalHeight() - win.GetHeight() || win.GetY() <= 0) {
            y_vel = -y_vel;
        }

        win.Move(win.GetX() + x_vel, win.GetY() + y_vel);

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        scr.Clear(foo);
    }
}

} // namespace LibTesix

// 🮲🮳
