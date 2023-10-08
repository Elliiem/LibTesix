#pragma once
#include "Screen.h"
#include "Style.h"
#include "StyledString.h"

#include <cinttypes>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

namespace LibTesix {

void Dev() {
    /*Screen src;

    Window win(10, 10, 10, 5);
    Style foo;
    foo.Blinking(1)->BG(Color(50, 50, 50))->Italic(1);
    win.Print(0, 0, "aaaaaaaaa", foo);
    win.Print(0, 1, "bbbbbbbbb", foo);
    win.Print(0, 2, "ccccccccc", foo);
    win.Print(0, 3, "ddddddddd", foo);
    win.Print(0, 4, "eeeeeeeee", foo);

    win.Draw(&src.state, 1);
    printf("\n");

    sleep(4);
    printf("\033[0m");*/

    StyledString string("");
    string.Append("1111", STANDARD_STYLE);
    Style foo;
    foo.BG(Color(255, 0, 0));
    string.Append("2222", foo);
    foo.BG(Color(0, 255, 0));
    string.Append("3333", foo);
    foo.BG(Color(0, 0, 255));
    string.Append("4444", foo);

    Style state;
    state.BG(Color(50, 50, 50));

    string.Erase(1, 5);

    string.UpdateRaw();
    printf(string.Raw(&state).c_str());
    string.StyleEnd().SetState(&state);

    printf("\033[0m");
}

} // namespace LibTesix