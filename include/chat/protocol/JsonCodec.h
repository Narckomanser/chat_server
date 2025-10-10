#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include <chat/protocol/Dto.h>

class JsonCodec
{
public:
    static std::string to_line(const Envelope& envelope);
    static std::optional<Envelope> from_line(std::string_view line);
};