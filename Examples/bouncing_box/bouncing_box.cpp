#include "LibTesix.h"

#include <cinttypes>
#include <iostream>
#include <thread>

int main() {
    LibTesix::InitScreen();

    LibTesix::Style background("background");
    background.BG(LibTesix::Color(0, 50, 0));

    const LibTesix::Style* background_p = LibTesix::style_allocator.Add(background);

    LibTesix::JsonDocument json("/home/elliem/Dev/Programs/1st-Party/cpp/LibTesix/Examples/bouncing_box/window.json");
    LibTesix::Window win(json, "window");

    LibTesix::Clear(background_p);

    int64_t x_vel = 2;
    int64_t y_vel = 1;

    while(true) {
        win.Draw(LibTesix::state);
        LibTesix::Update();

        if(win.GetX() + 1 >= LibTesix::GetTerminalWidth() - win.GetWidth() || win.GetX() <= 0) {
            x_vel = -x_vel;
        }

        if(win.GetY() >= LibTesix::GetTerminalHeight() - win.GetHeight() || win.GetY() <= 0) {
            y_vel = -y_vel;
        }

        win.Move(win.GetX() + x_vel, win.GetY() + y_vel);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        LibTesix::Clear(background_p);
    }

    return 0;
}