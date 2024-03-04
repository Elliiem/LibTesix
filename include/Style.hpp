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
    Color _fg;
    // background
    Color _bg;

    bool operator==(const ColorPair& other) const;
};

const ColorPair STANDARD_COLORPAIR(STANDARD_FG, STANDARD_BG);

struct Style {
    friend class StyleAllocator;

    // The indicies of modifiers in LibTesix::Style::bool_state
    enum States { BOLD, FAINT, BLINKING, REVERSE, UNDERLINED, ITALIC, STATES_COUNT };

    Style(const std::string& name);                // TODO make private
    Style(const std::string& name, ColorPair col); // TODO make private

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

    bool             GetMod(States state) const;
    const ColorPair& GetColor() const;

    // Returns the escape code sequence used in order to change from the supplied teminal state to this style
    std::string GetEscapeCode(const Style& state) const;

    void Reset();

    bool operator==(const Style& other) const;

  private:
    Style();

    //  States of modifiers eg. bold, italic or blinking text
    //  these modifiers are stored in this vector at the values in the enum States, defined in the Style source file
    std::bitset<STATES_COUNT> modifiers;

    // The name of the Style used to identify styles when exporting or importing from json
    std::string name;

    // The color of the Style
    ColorPair col;
};

class StyleAllocator {
  public:
    StyleAllocator();

    Style& operator[](const std::string& name);

    Style& Add(const std::string& name);

  private:
    std::map<std::string, std::unique_ptr<Style>> _styles;
};

// TODO let user define this
inline StyleAllocator style_allocator;
inline const Style&   STANDARD_STYLE = style_allocator["__default__"];

} // namespace LibTesix