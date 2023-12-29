#include "Style.h"

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
    this->fg = fg;
    this->bg = bg;
}

ColorPair::ColorPair() {
    fg = Color(255, 255, 255);
    bg = Color(0, 0, 0);
}

Style::Style() {
    name = "";
}

Style::Style(const std::string& name) {
    this->name = name;
}

Style::Style(const std::string& name, ColorPair col) {
    this->name = name;
    this->col = col;
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

    modifiers.reset();
}

StyleAllocator::StyleAllocator() {
    ids[""] = 0;
    styles.push_back(std::make_unique<const Style>(""));
}

const Style* StyleAllocator::operator[](const std::string& name) {
    if(ids.contains(name)) {
        uint64_t id = ids[name];
        return styles[id].get();
    }

    return nullptr;
}

const Style* StyleAllocator::operator[](uint64_t id) {
    return (id < styles.size()) ? styles[id].get() : nullptr;
}

const Style* StyleAllocator::Add(const Style& style) {
    if(!ids.contains(style.name)) {
        ids[style.name] = styles.size();

        styles.push_back(std::make_unique<const Style>(style));

        return styles.back().get();
    } else {
        return (*this)[style.name];
    }
}

} // namespace LibTesix
