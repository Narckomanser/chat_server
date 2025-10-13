#pragma once

#include <string>
#include <optional>
#include <vector>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

struct Envelope
{
    std::string type;
    std::optional<std::string> req_id;
    json payload;
    int64_t ts{0};

    static Envelope info(std::string text);
    static Envelope error(std::string code,std::string message);
    static Envelope hello(std::vector<std::string> caps);
    static Envelope auth_challenge(std::string realm, std::string nonce, int ttl);
};
