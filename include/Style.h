#pragma once

#include <cinttypes>
#include <cstring>
#include <hash_map>
#include <map>
#include <sparsehash/dense_hash_map>
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

    bool GetMod(States state) const;

    // Returns the escape code sequence used in order to change from the supplied teminal state to this style
    std::string GetEscapeCode(const Style& state) const;

    void Reset();

    // The color of the Style
    ColorPair col;

  private:
    //  States of modifiers eg. bold, italic or blinking text
    //  these modifiers are stored in this vector at the values in the enum States, defined in the Style source file
    std::vector<bool> bool_state;
};

class StyleAllocator {
  public:
    StyleAllocator();

    const Style* operator[](const char* name);
    const Style* operator[](uint64_t id);

    uint64_t Add(const Style& style, const char* name);

  private:
    struct eqstr {
        bool operator()(char const* a, char const* b) const {
            return std::strcmp(a, b) < 0;
        }
    };

    std::map<const char*, bool, eqstr> ids;
    std::vector<Style> styles;
};

inline StyleAllocator style_allocator;

} // namespace LibTesix