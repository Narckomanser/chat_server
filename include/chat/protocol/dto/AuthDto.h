#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace dto::auth
{
    struct Auth {std::string user;};
    struct AuthResp {std::string user; std::string response;};
    struct Challenge {std::string realm; std::string nonce; int ttl{60};};

    inline void from_json(const json& j, Auth& payload)
    {
        payload.user = j.at("user").get<std::string>();
    }

    inline void from_json(const json& j, AuthResp& payload)
    {
        payload.user = j.at("user").get<std::string>();
        payload.response = j.at("response").get<std::string>();
    }

    inline void to_json(json& j, const Challenge& payload)
    {
        j = json{{{"realm", payload.realm}},{{"nonce", payload.nonce}},{{"ttl", payload.ttl}}};
    }
} // namespace dto::auth