#include "StyledSegmentString.hpp"

namespace LibTesix {
class StyledString : StyledSegmentString {
  public:
    StyledString() = default;
    StyledString(const StyledString& str);

    using StyledSegmentString::Len;
#ifdef NDEBUG
    using StyledSegmentString::PrintDebug;

    void Append(const tiny_utf8::string& str, const Style& style);
    void Insert(const tiny_utf8::string& str, const Style& style, std::size_t index);
    void Erase(std::size_t start, std::size_t end);
#endif
};

} // namespace LibTesix
