#pragma once

#include "chat/infra/repos/UserRepository.h"
#include "chat/infra/security/NonceStore.h"
#include "chat/core/Crypto.h"

#include <optional>
#include <string>

class AuthService
{
public:
    AuthService(UserRepository& users, std::string realm, NonceStore& nonces)  //
        : users_(users), realm_(std::move(realm)), nonces_(nonces)
    {
    }

    std::optional<std::string> auth_challenge(std::string_view username);
    bool auth_complete(std::string_view username, std::string_view response_hex);

    const std::string& realm() const { return realm_; }

private:
    UserRepository& users_;
    std::string realm_;
    NonceStore& nonces_;
};