#pragma once

#include <cinttypes>
#include <string>
#include <vector>

namespace LibTesix {

struct Color {
    Color(uint64_t r, uint64_t g, uint64_t b);
    Color();

    bool operator==(const Color& other);

    uint64_t r;
    uint64_t g;
    uint64_t b;
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
    // The indicies of modifiers in LibTesix::Style::bool_state
    enum States { BOLD, FAINT, BLINKING, REVERSE, UNDERLINED, ITALIC, STATES_COUNT };

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

    bool operator[](States state) const;

    // Returns the escape code sequence used in order to change from the supplied teminal state to this style
    std::string GetEscapeCode(const Style& state);

    void Reset();

    // The color of the Style
    ColorPair col;

  private:
    //  States of modifiers eg. bold, italic or blinking text
    //  these modifiers are stored in this vector at the values in the enum States, defined in the Style source file
    std::vector<bool> bool_state;
};

const Style STANDARD_STYLE;
const Style NULL_STYLE(ColorPair(Color(-1, -1, -1), Color(-1, -1, -1)));

} // namespace LibTesix