#pragma once

#include "Json.h"
#include "Overlay.h"
#include "SegmentArray.h"
#include "Style.h"
#include "StyledString.h"
#include "Terminal.h"
#include "Window.h"

#include <cinttypes>
#include <iostream>
#include <thread>

namespace LibTesix {

void Dev() {
    InitScreen();

    Style foo;
    foo.Blinking(false)->BG(Color(50, 150, 50))->Bold(true);

    JsonDocument json("/home/elliem/Dev/Programs/1st-Party/cpp/LibTesix/examples/bouncing_box/window.json");
    Window win(json, "window");
    win.UpdateRaw();

    Clear(foo);

    int32_t x_vel = 2;
    int32_t y_vel = 1;

    while(true) {
        win.Draw(state, true);

        Update();

        if(win.GetX() + 1 >= GetTerminalWidth() - win.GetWidth() || win.GetX() <= 0) {
            x_vel = -x_vel;
        }

        if(win.GetY() >= GetTerminalHeight() - win.GetHeight() || win.GetY() <= 0) {
            y_vel = -y_vel;
        }

        win.Move(win.GetX() + x_vel, win.GetY() + y_vel);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Clear(foo);
    }
}

} // namespace LibTesix

// 🮲🮳
