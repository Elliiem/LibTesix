#include "StyledSegmentString.hpp"

namespace LibTesix {
class StyledString : private StyledSegmentString {
  public:
    StyledString() = default;
    StyledString(const StyledString& str);

    using StyledSegmentString::Len;
#ifdef NDEBUG
    using StyledSegmentString::PrintDebug;
#endif

    /** @brief Appends a segment to the string
     * @param str The contents of the new segment
     * @param style The style of the segment
     */
    void Append(const tiny_utf8::string& str, const Style& style);

    /**
     * @brief Inserts a segment at the index
     * @param str The contents of the segment
     * @param style The style of the segment
     * @param index The index(g) to insert to
     */
    void Insert(const tiny_utf8::string& str, const Style& style, std::size_t index);

    /**
     * @brief Erases a range in the string
     * @param start The start index(g) of the range
     * @param end The end index(g) of the range
     */
    void Erase(std::size_t start, std::size_t end);
};

} // namespace LibTesix
