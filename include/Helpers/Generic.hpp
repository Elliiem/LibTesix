#pragma once

#include <cstddef>
#include <vector>

namespace LibTesix {

/**
 * @brief Increment the given value with an upper bound.
 * @param x The value to be incremented.
 * @param max The upper bound limit.
 * @return The incremented value, constrained by the upper bound.
 */
inline std::size_t IncrBounded(std::size_t x, std::size_t max) {
    return x < max ? x + 1 : max;
}

/**
 * @brief Decrement the given value with a lower bound.
 * @param x The value to be decremented.
 * @param min The lower bound limit.
 * @return The decremented value, constrained by the lower bound.
 */
inline std::size_t DecrBounded(std::size_t x, std::size_t min) {
    return x > min ? x - 1 : min;
}

/**
 * @brief Get the maximum valid index for a given vector.
 * @tparam T The type of the vector elements.
 * @param vec The vector for which the maximum index is calculated.
 * @return The maximum index for the vector.
 */
template<typename T> inline std::size_t MaxIndex(const std::vector<T>& vec) {
    return vec.size() - 1;
}

/**
 * @brief Get the next element in a vector based on the current index.
 * @tparam T The type of the vector elements.
 * @param cur The current index.
 * @param vec The vector containing the elements.
 * @return Reference to the next element in the vector.
 */
template<typename T> inline T& GetNext(std::size_t cur, std::vector<T>& vec) {
    return vec[IncrBounded(cur, MaxIndex(vec))];
}

/**
 * @brief Get the next element in a const vector based on the current index.
 * @tparam T The type of the vector elements.
 * @param cur The current index.
 * @param vec The const vector containing the elements.
 * @return Reference to the next element in the const vector.
 */
template<typename T> inline const T& GetNext(std::size_t cur, const std::vector<T>& vec) {
    return vec[IncrBounded(cur, MaxIndex(vec))];
}

/**
 * @brief Get the previous element in a vector based on the current index.
 * @tparam T The type of the vector elements.
 * @param cur The current index.
 * @param vec The vector containing the elements.
 * @return Reference to the previous element in the vector.
 */
template<typename T> inline T& GetPrev(std::size_t cur, std::vector<T>& vec) {
    return vec[DecrBounded(cur, 0)];
}

/**
 * @brief Get the previous element in a const vector based on the current index.
 * @tparam T The type of the vector elements.
 * @param cur The current index.
 * @param vec The const vector containing the elements.
 * @return Reference to the previous element in the const vector.
 */
template<typename T> inline const T& GetPrev(std::size_t cur, const std::vector<T>& vec) {
    return vec[DecrBounded(cur, 0)];
}

} // namespace LibTesix