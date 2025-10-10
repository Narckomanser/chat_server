#pragma once

#include <string>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>
#include <chat/protocol/MessageTypes.h>

using json = nlohmann::json;

struct Envelope
{
    MsgType type{MsgType::Unknown};
    std::optional<std::string> req_id;
    json payload;
    int64_t ts{0};

    static Envelope info(std::string text);
    static Envelope error(std::string code,std::string message);
    static Envelope hello(std::vector<std::string> caps);
    static Envelope auth_challenge(std::string realm, std::string nonce, int ttl);
};

struct AuthReq {std::string user;};
struct AuthRespReq {std::string user; std::string response;};
struct InfoPayload {std::string text;};
struct ErrorPayload {std::string code; std::string message;};
struct AuthChallengePayload {std::string realm; std::string nonce; int ttl{60};};


inline void to_json(json& j, const InfoPayload& payload)
{
    j = json{"text", payload.text};
}

inline void to_json(json& j, const ErrorPayload& payload)
{
    j = json{{{"code", payload.code}},{{"message", payload.message}}};
}

inline void to_json(json& j, const AuthChallengePayload& payload)
{
    j = json{{{"realm", payload.realm}},{{"nonce", payload.nonce}},{{"ttl", payload.ttl}}};
}

inline void from_json(const json& j, AuthReq& payload)
{
    payload.user = j.at("user").get<std::string>();
}

inline void from_json(const json& j, AuthRespReq& payload)
{
    payload.user = j.at("user").get<std::string>();
    payload.response = j.at("response").get<std::string>();
}
