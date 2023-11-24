#include "LibTesix.h"

#include <cinttypes>
#include <iostream>
#include <thread>

int main() {
    LibTesix::InitScreen();

    LibTesix::Style background;
    background.BG(LibTesix::Color(50, 150, 50));

    LibTesix::JsonDocument json("/home/elliem/Dev/Programs/1st-Party/cpp/LibTesix/Examples/bouncing_box/window.json");
    LibTesix::Window win(json, "window");

    Clear(background);

    int32_t x_vel = 2;
    int32_t y_vel = 1;

    while(true) {
        win.Draw(LibTesix::state, true);
        LibTesix::Update();

        if(win.GetX() + 1 >= LibTesix::GetTerminalWidth() - win.GetWidth() || win.GetX() <= 0) {
            x_vel = -x_vel;
        }

        if(win.GetY() >= LibTesix::GetTerminalHeight() - win.GetHeight() || win.GetY() <= 0) {
            y_vel = -y_vel;
        }

        win.Move(win.GetX() + x_vel, win.GetY() + y_vel);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Clear(background);
    }

    return 0;
}