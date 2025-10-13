#include <chat/app/handlers/AuthHandlers.h>
#include <chat/protocol/JsonCodec.h>
#include <chat/protocol/dto/Dto.h>

std::string AuthHandlers::Auth(const dto::auth::Auth& req, SessionCtx& ctx)
{
    auto nonce = svc_.AuthChallenge(req.user);
    if (!nonce) return JsonCodec::to_line(Envelope::error("unknown_user", "user not found"));

    ctx.username = req.user;
    return JsonCodec::to_line(Envelope::auth_challenge(svc_.realm(), *nonce, 60));
}

std::string AuthHandlers::AuthResp(const dto::auth::AuthResp& req, SessionCtx& ctx)
{
    if (!svc_.AuthComplete(req.user, req.response)) return JsonCodec::to_line(Envelope::error("auth_failed", "bad credentials"));

    ctx.authenticated = true;
    ctx.username = req.user;
    return JsonCodec::to_line(Envelope::info("authentication successful"));
}
