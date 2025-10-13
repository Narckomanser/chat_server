#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace dto::chat
{
    struct Msg {std::string text;};
    struct Event {std::string room; std::string from; std::string text; };

    inline void from_json(const json& j, Msg& payload)
    {
        payload.text = j.at("text").get<std::string>();
    }

    inline void to_json(json& j, const Event& payload)
    {
        j = {{"room", payload.room},{"from", payload.from},{"text", payload.text}};
    }
} // namespace dto::rooms