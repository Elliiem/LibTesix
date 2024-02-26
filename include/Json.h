#pragma once

#include "SegmentArray.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace LibTesix {

struct JsonDocument {
    JsonDocument(const std::string& filepath);

    rapidjson::Document doc;
    std::string filepath;

    void Save();
};

SegmentArray ReadSegmentArray(rapidjson::Value& json_arr);

} // namespace LibTesix