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
    /*Screen scr;

    Style foo;
    foo.Blinking(false)->BG(Color(50, 50, 50))->Bold(true);

    Window win(10, 10, 10, 5);

    win.Write(0, 0, "#################################################", foo);

    Style background;
    background.BG(Color(0, 0, 50));
    scr.Clear(background);

    int32_t x_vel = 2;
    int32_t y_vel = 1;

    while(true) {
        win.Draw(scr.state);

        scr.Update();

        if(win.GetX() >= GetTerminalWidth() - win.GetWidth() || win.GetX() <= 0) {
            x_vel = -x_vel;
        }

        if(win.GetY() >= GetTerminalHeight() - win.GetHeight() || win.GetY() <= 0) {
            y_vel = -y_vel;
        }

        win.Move(win.GetX() + x_vel, win.GetY() + y_vel);

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        scr.Clear(background);
    }*/

    StyledSegmentArray foo;

    foo.segments.push_back(StyledSegment("foo", STANDARD_STYLE, 1));

    foo.segments.push_back(StyledSegment("foo", STANDARD_STYLE, 10));

    foo.PrintDebug();

    foo.Write("xxx", STANDARD_STYLE, 1);
    foo.Write("amogert!", STANDARD_STYLE, 3);

    foo.PrintDebug();
}

} // namespace LibTesix

// 🮲🮳