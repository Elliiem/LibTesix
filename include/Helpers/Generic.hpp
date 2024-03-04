#pragma once

#include <cstddef>
#include <vector>

namespace LibTesix {

inline std::size_t IncrBounded(std::size_t x, std::size_t max) {
    return x < max ? x + 1 : max;
}

inline std::size_t DecrBounded(std::size_t x, std::size_t min) {
    return x > min ? x - 1 : min;
}

template<typename T> inline std::size_t MaxIndex(const std::vector<T>& vec) {
    return vec.size() - 1;
}

template<typename T> inline T& GetNext(std::size_t cur, std::vector<T>& vec) {
    return vec[IncrBounded(cur, MaxIndex(vec))];
}

template<typename T> inline const T& GetNext(std::size_t cur, const std::vector<T>& vec) {
    return vec[IncrBounded(cur, MaxIndex(vec))];
}

template<typename T> inline T& GetPrev(std::size_t cur, std::vector<T>& vec) {
    return vec[DecrBounded(cur, 0)];
}

template<typename T> inline const T& GetPrev(std::size_t cur, const std::vector<T>& vec) {
    return vec[DecrBounded(cur, 0)];
}

} // namespace LibTesix