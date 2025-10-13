#pragma once

#include <string_view>

namespace proto::info
{
    enum class Type
    {
        Hello,
        Info,
        Error,
        Quit
    };

    inline std::string_view to_string(Type t)
    {
        switch (t)
        {
            case Type::Hello: return "hello";
            case Type::Info: return "info";
            case Type::Error: return "error";
            case Type::Quit: return "quit";
        }

        return "unknown";
    }
}  // namespace proto::info
