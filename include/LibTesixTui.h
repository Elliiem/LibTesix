#pragma once
#include "Overlay.h"
#include "Screen.h"
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

    Overlay foo(10, 5);
    foo.lines[0].string.push_back(StyledSegment("bar", STANDARD_STYLE, 1));
    foo.lines[0].string.push_back(StyledSegment("bar", STANDARD_STYLE, 10));

    icu::UnicodeString tst("bar");

    StyledString test_line;
    test_line.Resize(13);
    test_line.Write(tst, STANDARD_STYLE, 1);
    test_line.Write(tst, STANDARD_STYLE, 10);

    // foo.PrintDebug();

    uint32_t x = 0;
    uint32_t len = 4;

    printf("%i\n", foo.HitsSegment(x, 0, len));

    Style colored;
    colored.BG(Color(255, 0, 0));
    test_line.Write(std::string(len, ' ').c_str(), colored, x);

    Style state = NULL_STYLE;
    printf("\n");
    test_line.Print(state);
    printf("|\033[0m\n");
}

} // namespace LibTesix

// 🮲🮳