#include "Style.h"

#include <iostream>

namespace LibTesix {

// This stores the escape codes to achieve the modifiers enumerated in States found above
// each value has two corresponding escape codes the first one is to turn that modifier off the second one to turn it on
// To get the corresponding index from a modifier to a escape code use this formula: 2 * modifier + bool(false = off, true = on)
const std::vector<std::string> ESCAPE_CODES = {
    "\033[22m",
    "\033[1m",
    "\033[22m",
    "\033[2m",
    "\033[23m",
    "\033[3m",
    "\033[24m",
    "\033[4m",
    "\033[25m",
    "\033[5m",
    "\033[27m",
    "\033[7m",
};

Color::Color(uint32_t r, uint32_t g, uint32_t b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color() {
    r = 0;
    g = 0;
    b = 0;
}

bool Color::operator==(const Color& other) {
    return (r == other.r) && (g == other.g) && (b == other.b);
}

ColorPair::ColorPair(Color fg, Color bg) {
    this->fg = fg;
    this->bg = bg;
}

ColorPair::ColorPair() {
    fg = Color(255, 255, 255);
    bg = Color(0, 0, 0);
}

Style::Style() {
    bool_state.resize(STATES_COUNT, false);
}

Style::Style(ColorPair col) {
    this->col = col;
    bool_state.resize(STATES_COUNT, false);
}

Style* Style::Bold(bool val) {
    if(bool_state[FAINT] && val) bool_state[FAINT] = false;
    bool_state[BOLD] = val;
    return this;
}

Style* Style::Faint(bool val) {
    if(bool_state[BOLD] && val) bool_state[BOLD] = false;
    bool_state[FAINT] = val;
    return this;
}

Style* Style::Blinking(bool val) {
    bool_state[BLINKING] = val;
    return this;
}

Style* Style::Reverse(bool val) {
    bool_state[REVERSE] = val;
    return this;
}

Style* Style::Underlined(bool val) {
    bool_state[UNDERLINED] = val;
    return this;
}

Style* Style::Italic(bool val) {
    bool_state[ITALIC] = val;
    return this;
}

Style* Style::BG(LibTesix::Color val) {
    col.bg = val;
    return this;
}

Style* Style::FG(LibTesix::Color val) {
    col.fg = val;
    return this;
}

Style* Style::Color(ColorPair val) {
    col = val;
    return this;
}

std::string Style::GetEscapeCode(const Style& state) {
    std::vector<std::pair<uint32_t, bool>> bool_changes(STATES_COUNT);
    uint32_t change_count = 0;

    for(uint32_t i = 0; i < STATES_COUNT; i++) {
        if(bool_state[i] != state.bool_state[i]) {
            bool_changes[change_count] = std::pair<uint32_t, bool>(i, bool_state[i]);
            change_count++;
        }
    }

    bool_changes.resize(change_count);

    std::string ret;

    for(std::pair<uint32_t, bool> change : bool_changes) {
        ret.append(ESCAPE_CODES[2 * change.first + change.second]);
    }

    if(!(col.fg == state.col.fg)) {
        ret.append("\033[38;2;" + std::to_string(col.fg.r) + ";" + std::to_string(col.fg.g) + ";" + std::to_string(col.fg.b) + "m");
    }

    if(!(col.bg == state.col.bg)) {
        ret.append("\033[48;2;" + std::to_string(col.bg.r) + ";" + std::to_string(col.bg.g) + ";" + std::to_string(col.bg.b) + "m");
    }

    return ret;
}

void Style::Reset() {
    col = STANDARD_COLORPAIR;
    bool_state = STANDARD_STYLE.bool_state;
}

} // namespace LibTesix
