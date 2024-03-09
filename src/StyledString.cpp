#include "StyledString.hpp"

#include "Helpers/Generic.hpp"
#include "Helpers/StyledSegmentString.hpp"

namespace LibTesix {
StyledString::StyledString(const StyledString& str) : StyledSegmentString(str) {
}

void StyledString::Append(const tiny_utf8::string& str, const Style& style) {
    StyledSegmentString::Append(str, style);
}

void StyledString::Insert(const tiny_utf8::string& str, const Style& style, std::size_t index) {
    if(index > Len()) throw std::range_error("Index is out of bounds! << StyledString::Insert()");

    StyledSegmentString::Insert(str, style, index);
}

void StyledString::Erase(std::size_t start, std::size_t end) {
    if(start >= Len()) throw std::range_error("Start is out bounds! << StyledString::Erase()");

    std::size_t start_index = GetSegmentIndex(start);

    bool at_start    = start == _segments[start_index]->_start;
    bool at_end      = end + 1 == _segments[start_index]->_start + _segments[start_index]->_str.length();
    bool erases_ends = at_start || at_end;

    StyledSegmentString::Erase(start, end);

    uint64_t erased_len = end - start + 1;

    for(std::size_t i = start_index + 1; i < _segments.size();) {
        bool merged_on_move = MoveSegment(start_index + i, _segments[start_index + i]->_start - erased_len);

        if(!merged_on_move) {
            i++;
        }
    }
}

} // namespace LibTesix