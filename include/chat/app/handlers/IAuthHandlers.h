#pragma once
#include <string>
#include <chat/protocol/SessionCtx.h>
#include <chat/protocol/dto/AuthDto.h>

class IAuthHandlers
{
public:
    virtual ~IAuthHandlers() = default;

    virtual std::string Auth(const dto::auth::Auth& req, SessionCtx& ctx) = 0;

    virtual std::string AuthResp(const dto::auth::AuthResp& req, SessionCtx& ctx) = 0;
};
