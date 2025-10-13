#include "chat/app/services/AuthService.h"

std::optional<std::string> AuthService::AuthChallenge(std::string_view username)
{
    if (!users_.Exists(std::string(username))) return std::nullopt;
    auto nonce = crypto::random_hex();
    nonces_.put(std::string(username), nonce);

    return nonce;
}

bool AuthService::AuthComplete(std::string_view username, std::string_view response_hex)
{
    auto user = users_.Get(std::string(username));
    if (!user) return false;

    auto nonce = nonces_.consume(std::string(username));
    if (!nonce) return false;

    const std::string expected = crypto::md5_hex(user->ha1 + ":" + *nonce);

    return expected == response_hex;
}
