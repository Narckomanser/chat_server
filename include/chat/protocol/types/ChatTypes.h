#pragma once

#include <string_view>

namespace proto::chat
{
    enum class Type
    {
        Msg,
        Event
    };

    inline std::string_view to_string(Type t)
    {
        switch (t)
        {
            case Type::Msg: return "msg";
            case Type::Event: return "event";
        }

        return "unknown";
    }
}  // namespace proto::chat
