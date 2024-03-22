#pragma once

#include "StyledString.hpp"

namespace LibTesix {

void Draw(const StyledString<NCONT>& str, uint64_t x, uint64_t y) {
    printf("unsus\n");
}

void Draw(const StyledString<CONT>& str, uint64_t x, uint64_t y) {
    printf("sus\n");
}

template<typename T> struct DrawQueue {};

template<> struct DrawQueue<StyledString<CONT>> {};

template<> struct DrawQueue<StyledString<NCONT>> {};

} // namespace LibTesix
