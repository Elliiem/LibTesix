#pragma once

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace LibTesix {

struct JsonDocument {
    JsonDocument(const char* filepath);

    rapidjson::Document doc;
    std::string filepath;

    void Save();
};

} // namespace LibTesix