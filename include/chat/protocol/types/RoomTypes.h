#pragma once

#include <string_view>

namespace proto::room
{
    enum class Type
    {
        Join,
        Leave,
        Rooms,
        Event
    };

    inline std::string_view to_string(Type t)
    {
        switch (t)
        {
            case Type::Join: return "join";
            case Type::Leave: return "leave";
            case Type::Rooms: return "rooms";
            case Type::Event: return "event";
        }

        return "unknown";
    }
}  // namespace proto::room
