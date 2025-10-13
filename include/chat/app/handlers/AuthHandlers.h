#pragma once
#include <memory>
#include <string>
#include <chat/app/handlers/IAuthHandlers.h>
#include <chat/app/services/AuthService.h>

class AuthHandlers final : public IAuthHandlers
{
public:
    explicit AuthHandlers(AuthService& svc) : svc_(svc) {}

    std::string Auth(const dto::auth::Auth& req, SessionCtx& ctx) override;
    std::string AuthResp(const dto::auth::AuthResp& req, SessionCtx& ctx) override;

private:
    AuthService& svc_;
};
