#pragma once

#include <bitset>
#include <cinttypes>
#include <cstring>
#include <map>
#include <memory>
#include <vector>

namespace LibTesix {

struct Color {
    Color(uint64_t r, uint64_t g, uint64_t b);
    Color();

    bool operator==(const Color& other) const;

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
    friend class StyleAllocator;

    // The indicies of modifiers in LibTesix::Style::bool_state
    enum States { BOLD, FAINT, BLINKING, REVERSE, UNDERLINED, ITALIC, STATES_COUNT };

    Style(const std::string& name);
    Style(const std::string& name, ColorPair col);

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

    bool GetMod(States state) const;

    // Returns the escape code sequence used in order to change from the supplied teminal state to this style
    std::string GetEscapeCode(const Style& state) const;

    void Reset();

    // The color of the Style
    ColorPair col;

  private:
    Style();

    //  States of modifiers eg. bold, italic or blinking text
    //  these modifiers are stored in this vector at the values in the enum States, defined in the Style source file
    std::bitset<STATES_COUNT> modifiers;
    std::string name;
};

class StyleAllocator {
  public:
    StyleAllocator();

    const Style* operator[](const std::string& name);
    const Style* operator[](uint64_t id);

    const Style* Add(const Style& style);

  private:
    std::map<std::string, uint64_t> ids;
    std::vector<std::unique_ptr<const Style>> styles;
};

inline StyleAllocator style_allocator;
inline const Style* STANDARD_STYLE = style_allocator[""];

} // namespace LibTesix