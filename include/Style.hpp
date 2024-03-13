#pragma once

#include <bitset>
#include <cinttypes>
#include <cstring>
#include <map>
#include <memory>
#include <tinyutf8/tinyutf8.h>
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
    Color _fg;
    // background
    Color _bg;

    bool operator==(const ColorPair& other) const;
};

const ColorPair STANDARD_COLORPAIR(STANDARD_FG, STANDARD_BG);

class Style {
    friend class StyleAllocator;

  public:
    // The indicies of _modifiers in LibTesix::Style::bool_state
    enum States { BOLD, FAINT, BLINKING, REVERSE, UNDERLINED, ITALIC, STATES_COUNT };

    // Setters for _modifiers
    Style* Bold(bool val);
    Style* Faint(bool val);
    Style* Blinking(bool val);
    Style* Reverse(bool val);
    Style* Underlined(bool val);
    Style* Italic(bool val);
    Style* BG(Color val);
    Style* FG(Color val);
    Style* Color(ColorPair val);

    bool             GetMod(States state) const;
    const ColorPair& GetColor() const;

    // Returns the escape code sequence used in order to change from the supplied teminal state to this style
    std::string GetEscapeCode(const Style& state) const;

    void Reset();

    bool operator==(const Style& other) const;

  private:
    Style(const tiny_utf8::string& name);

    //  States of modifiers eg. bold, italic or blinking text
    //  these modifiers are stored in this vector at the values in the States enum
    std::bitset<STATES_COUNT> _modifiers;

    // The name of the Style used to identify styles when exporting or importing from json
    const tiny_utf8::string _name;

    // The color of the Style
    ColorPair _col;
};

class StyleAllocator {
  public:
    StyleAllocator();

    Style& operator[](const tiny_utf8::string& name);

    Style& Add(const tiny_utf8::string& name);

  private:
    std::map<tiny_utf8::string, std::unique_ptr<Style>> _styles;
};

} // namespace LibTesix