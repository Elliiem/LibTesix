#include "Style.hpp"

#include <cstring>
#include <iostream>

namespace LibTesix {

// This stores the escape codes to achieve the _modifiers enumerated in States found above
// each value has two corresponding escape codes the first one is to turn that modifier off the second one to turn it on
// To get the corresponding index from a modifier to a escape code use this formula: 2 * modifier + bool(false = off,
// true = on)
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

Style::Style(const tiny_utf8::string& name) : _name(name) {
}

Style* Style::Bold(bool val) {
    if(_modifiers[FAINT] && val) _modifiers[FAINT] = false;
    _modifiers[BOLD] = val;
    return this;
}

Style* Style::Faint(bool val) {
    if(_modifiers[BOLD] && val) _modifiers[BOLD] = false;
    _modifiers[FAINT] = val;
    return this;
}

Style* Style::Blinking(bool val) {
    _modifiers[BLINKING] = val;
    return this;
}

Style* Style::Reverse(bool val) {
    _modifiers[REVERSE] = val;
    return this;
}

Style* Style::Underlined(bool val) {
    _modifiers[UNDERLINED] = val;
    return this;
}

Style* Style::Italic(bool val) {
    _modifiers[ITALIC] = val;
    return this;
}

Style* Style::BG(LibTesix::Color val) {
    _col._bg = val;
    return this;
}

Style* Style::FG(LibTesix::Color val) {
    _col._fg = val;
    return this;
}

Style* Style::Color(ColorPair val) {
    _col = val;
    return this;
}

bool Style::GetMod(States state) const {
    return _modifiers[state];
}

const ColorPair& Style::GetColor() const {
    return _col;
}

std::string Style::GetEscapeCode(const Style& state) const {
    std::vector<std::pair<uint64_t, bool>> bool_changes(STATES_COUNT);

    for(int64_t i = 0; i < STATES_COUNT; i++) {
        if(_modifiers[i] != state.GetMod(static_cast<States>(i))) {
            bool_changes.emplace_back(i, _modifiers[i]);
        }
    }

    std::string ret;

    for(std::pair<uint64_t, bool> change : bool_changes) {
        ret.append(ESCAPE_CODES[2 * change.first + change.second]);
    }

    if(!(_col._fg == state._col._fg)) {
        ret.append("\033[38;2;" + std::to_string(_col._fg.r) + ";" + std::to_string(_col._fg.g) + ";" +
                   std::to_string(_col._fg.b) + "m");
    }

    if(!(_col._bg == state._col._bg)) {
        ret.append("\033[48;2;" + std::to_string(_col._bg.r) + ";" + std::to_string(_col._bg.g) + ";" +
                   std::to_string(_col._bg.b) + "m");
    }

    return ret;
}

void Style::Reset() {
    _col = STANDARD_COLORPAIR;

    _modifiers.reset();
}

bool Style::operator==(const Style& other) const {
    return _col == other._col && _modifiers == other._modifiers && _name == other._name;
}

StyleAllocator::StyleAllocator() {
    _styles["__default__"] = std::make_unique<Style>(Style("__default__"));
}

Style& StyleAllocator::operator[](const tiny_utf8::string& name) {
    if(_styles.contains(name)) {
        return *_styles[name].get();
    }

    throw std::runtime_error("Unkown style! << StyleAllocator::operator[]");
}

Style& StyleAllocator::Add(const tiny_utf8::string& name) {
    if(_styles.contains(name)) {
        return *_styles[name].get();
    } else {
        _styles[name] = std::make_unique<Style>(Style(name));
        return *_styles[name].get();
    }
}

} // namespace LibTesix
