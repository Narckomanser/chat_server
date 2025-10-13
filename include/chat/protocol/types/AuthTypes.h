#pragma once

#include <string_view>

namespace proto::auth
{
    enum class Type
    {
        Auth,
        AuthChallenge,
        AuthResp
    };

    inline std::string_view to_string(Type t)
    {
        switch (t)
        {
            case Type::Auth: return "auth";
            case Type::AuthChallenge: return "auth_challenge";
            case Type::AuthResp: return "auth_resp";
        }

        return "unknown";
    }
}  // namespace proto::auth
