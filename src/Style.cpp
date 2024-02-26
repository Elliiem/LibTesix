#include "Style.hpp"

#include <cstring>
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
    "\033[25m",
    "\033[5m",
    "\033[27m",
    "\033[7m",
    "\033[24m",
    "\033[4m",
    "\033[23m",
    "\033[3m",
};

Color::Color(uint64_t r, uint64_t g, uint64_t b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color() {
    r = 0;
    g = 0;
    b = 0;
}

bool Color::operator==(const Color& other) const {
    return (r == other.r) && (g == other.g) && (b == other.b);
}

ColorPair::ColorPair(Color fg, Color bg) {
    this->_fg = fg;
    this->_bg = bg;
}

ColorPair::ColorPair() {
    _fg = Color(255, 255, 255);
    _bg = Color(0, 0, 0);
}

bool ColorPair::operator==(const ColorPair& other) const {
    return _fg == other._fg && _bg == other._bg;
}

Style::Style() {
    name = "";
}

Style::Style(const std::string& name) {
    this->name = name;
}

Style::Style(const std::string& name, ColorPair col) {
    this->name = name;
    this->col  = col;
}

Style* Style::Bold(bool val) {
    if(modifiers[FAINT] && val) modifiers[FAINT] = false;
    modifiers[BOLD] = val;
    return this;
}

Style* Style::Faint(bool val) {
    if(modifiers[BOLD] && val) modifiers[BOLD] = false;
    modifiers[FAINT] = val;
    return this;
}

Style* Style::Blinking(bool val) {
    modifiers[BLINKING] = val;
    return this;
}

Style* Style::Reverse(bool val) {
    modifiers[REVERSE] = val;
    return this;
}

Style* Style::Underlined(bool val) {
    modifiers[UNDERLINED] = val;
    return this;
}

Style* Style::Italic(bool val) {
    modifiers[ITALIC] = val;
    return this;
}

Style* Style::BG(LibTesix::Color val) {
    col._bg = val;
    return this;
}

Style* Style::FG(LibTesix::Color val) {
    col._fg = val;
    return this;
}

Style* Style::Color(ColorPair val) {
    col = val;
    return this;
}

bool Style::GetMod(States state) const {
    return modifiers[state];
}

std::string Style::GetEscapeCode(const Style& state) const {
    std::vector<std::pair<uint64_t, bool>> bool_changes(STATES_COUNT);

    for(int64_t i = 0; i < STATES_COUNT; i++) {
        if(modifiers[i] != state.GetMod(static_cast<States>(i))) {
            bool_changes.emplace_back(i, modifiers[i]);
        }
    }

    std::string ret;

    for(std::pair<uint64_t, bool> change : bool_changes) {
        ret.append(ESCAPE_CODES[2 * change.first + change.second]);
    }

    if(!(col._fg == state.col._fg)) {
        ret.append(
            "\033[38;2;" + std::to_string(col._fg.r) + ";" + std::to_string(col._fg.g) + ";" + std::to_string(col._fg.b) + "m");
    }

    if(!(col._bg == state.col._bg)) {
        ret.append(
            "\033[48;2;" + std::to_string(col._bg.r) + ";" + std::to_string(col._bg.g) + ";" + std::to_string(col._bg.b) + "m");
    }

    return ret;
}

void Style::Reset() {
    col = STANDARD_COLORPAIR;

    modifiers.reset();
}

bool Style::operator==(const Style& other) const {
    return col == other.col && modifiers == other.modifiers && name == other.name;
}

StyleAllocator::StyleAllocator() {
    _styles["__default__"] = std::make_unique<Style>("__default__");
}

Style& StyleAllocator::operator[](const std::string& name) {
    if(_styles.contains(name)) {
        return *_styles[name].get();
    }

    throw std::runtime_error("Unkown style! << StyleAllocator::operator[]");
}

Style& StyleAllocator::Add(const std::string& name) {
    if(_styles.contains(name)) {
        return *_styles[name].get();
    } else {
        _styles[name] = std::make_unique<Style>(name);
        return *_styles[name].get();
    }
}

} // namespace LibTesix
