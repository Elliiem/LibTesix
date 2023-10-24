#pragma once

#include <cinttypes>
#include <string>
#include <vector>

namespace LibTesix {

struct Color {
    Color(uint32_t r, uint32_t g, uint32_t b);
    Color();

    bool operator==(Color& other);

    uint32_t r;
    uint32_t g;
    uint32_t b;
};

const Color STANDARD_FG(255, 255, 255);
const Color STANDARD_BG(0, 0, 0);

struct ColorPair {
    ColorPair(Color fg, Color bg);
    ColorPair();

    // foreground
    Color fg;
    // background
    Color bg;
};

const ColorPair STANDARD_COLORPAIR(STANDARD_FG, STANDARD_BG);

struct Style {
    Style();
    Style(ColorPair col);

    // Setters for modifiers
    Style* Bold(bool val);
    Style* Faint(bool val);
    Style* Blinking(bool val);
    Style* Reverse(bool val);
    Style* Underlined(bool val);
    Style* Italic(bool val);
    Style* BG(Color val);
    Style* FG(Color val);
    Style* Color(ColorPair val);

    // Sets the provided state to this style
    void SetState(Style& state);

    // Returns the escape code sequence used in order to change from the supplied teminal state to this style
    std::string GetEscapeCode(Style& state);

    void Reset();

    // private:
    //  States of modifiers eg. bold, italic or blinking text
    //  these modifiers are stored in this vector at the values in the enum States, defined in the Style source file
    std::vector<bool> bool_state;

    // The color of the Style
    ColorPair col;
};

const Style STANDARD_STYLE;
const Style NULL_STYLE(ColorPair(Color(-1, -1, -1), Color(-1, -1, -1)));

} // namespace LibTesix