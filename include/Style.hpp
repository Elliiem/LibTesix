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
    Color(uint64_t r, uint64_t g, uint64_t b) {
        _r = r;
        _g = g;
        _b = b;
    }

    Color() {
        _r = 0;
        _g = 0;
        _b = 0;
    }

    uint8_t _r;
    uint8_t _g;
    uint8_t _b;

    bool operator==(const Color& other) const {
        return _r == other._r && _g == other._g && _b == other._b;
    }
};

const Color DEFAULT_FG(255, 255, 255);
const Color DEFAULT_BG(0, 0, 0);

struct ColorPair {
    ColorPair(Color fg, Color bg) {
        _fg = fg;
        _bg = bg;
    }

    ColorPair() = default;

    Color _fg;
    Color _bg;

    bool operator==(const ColorPair& other) const {
        return _fg == other._fg && _bg == other._bg;
    }
};

const ColorPair DEFAULT_COLOR(DEFAULT_FG, DEFAULT_BG);

enum Modifiers { BOLD, FAINT, BLINKING, REVERSE, UNDERLINED, ITALIC, MODIFIERS_COUNT };

class Style {
    friend class StyleAllocator;

  private:
    //  States of modifiers eg. bold, italic or blinking text
    //  these modifiers are stored in this vector at the values in the Modifiers enum
    std::bitset<Modifiers::MODIFIERS_COUNT> _modifiers;

    // The name of the Style used to identify styles when exporting or importing from json
    const tiny_utf8::string _name;

    // The color of the Style
    ColorPair _color;

  public:
    // Setters for _modifiers
    Style& Bold(bool val) {
        if(_modifiers[Modifiers::FAINT] && val) _modifiers[Modifiers::FAINT] = false;
        _modifiers[Modifiers::BOLD] = val;
        return *this;
    }

    Style& Faint(bool val) {
        if(_modifiers[Modifiers::BOLD] && val) _modifiers[Modifiers::BOLD] = false;
        _modifiers[Modifiers::FAINT] = val;
        return *this;
    }

    Style& Blinking(bool val) {
        _modifiers[Modifiers::BLINKING] = val;
        return *this;
    }

    Style& Reverse(bool val) {
        _modifiers[Modifiers::REVERSE] = val;
        return *this;
    }

    Style& Underlined(bool val) {
        _modifiers[Modifiers::UNDERLINED] = val;
        return *this;
    }

    Style& Italic(bool val) {
        _modifiers[Modifiers::ITALIC] = val;
        return *this;
    }

    Style& BG(Color val) {
        _color._bg = val;
        return *this;
    }

    Style& FG(Color val) {
        _color._fg = val;
        return *this;
    }

    Style& Color(ColorPair val) {
        _color = val;
        return *this;
    }

    bool GetMod(Modifiers state) const {
        return _modifiers[state];
    }

    const ColorPair& GetColor() const {
        return _color;
    }

    void Reset() {
        _modifiers.reset();
        _color = DEFAULT_COLOR;
    }

    bool operator==(const Style& other) const {
        return this == &other || _modifiers == other._modifiers && _color == other._color && _name == other._name;
    }

  private:
    Style(const tiny_utf8::string& name) : _name(name) {
    }
};

class StyleAllocator {
  public:
    StyleAllocator() = default;

  private:
    std::map<tiny_utf8::string, std::unique_ptr<Style>> _styles;

  public:
    Style& operator[](const tiny_utf8::string& name) {
        if(_styles.contains(name)) {
            return *_styles[name].get();
        }

        throw std::runtime_error("Unkown style! << StyleAllocator::operator[]");
    }

    Style& Add(const tiny_utf8::string& name) {
        if(_styles.contains(name)) {
            return *_styles[name].get();
        } else {
            _styles[name] = std::make_unique<Style>(Style(name));
            return *_styles[name].get();
        }
    }
};

} // namespace LibTesix