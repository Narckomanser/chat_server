#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace dto::rooms
{
    struct Join {std::string room;};
    struct Empty {};
    struct RoomEntry {std::string name; int members{}; };
    struct RoomsList {std::vector<RoomEntry> rooms; };
    struct Event {std::string kind; std::string room; std::string nick; };

    inline void from_json(const json& j, Join& payload)
    {
        payload.room = j.at("room").get<std::string>();
    }

    inline void to_json(json& j, const RoomsList& payload)
    {
        j = json::array();
        for (auto& room : payload.rooms)
        {
            j.push_back({{"name",room.name},{"members",room.members}});
        }
    }

    inline void to_json(json& j, const Event& payload)
    {
        j = {{"kind", payload.kind},{"room", payload.room},{"nick", payload.nick}};
    }
} // namespace dto::rooms