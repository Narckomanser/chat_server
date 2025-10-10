#include <chat/protocol/Router.h>
#include <chat/protocol/JsonCodec.h>
#include <chat/app/services/AuthService.h>
#include <chat/protocol/Dto.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static std::string to_line(const Envelope& e)
{
    return JsonCodec::to_line(e);
}

std::string Router::handle(const std::string& json_line, bool& authenticated, std::string& username)
{
    auto env = JsonCodec::from_line(json_line);
    if (!env) return to_line(Envelope::error("bad_json", "cannot parse"));

    switch (env->type)
    {
        case MsgType::Quit: return to_line(Envelope::info("bye"));

        case MsgType::Auth:
        {
            // payload → DTO
            AuthReq req{};
            try
            {
                req = env->payload.get<AuthReq>();
            }
            catch (...)
            {
                return to_line(Envelope::error("usage", "auth: need user"));
            }

            auto nonce_opt = auth_->auth_challenge(req.user);
            if (!nonce_opt) return to_line(Envelope::error("unknown_user", "user not found"));
            username = req.user;
            return to_line(Envelope::auth_challenge(auth_->realm(), *nonce_opt, 60));
        }

        case MsgType::AuthResp:
        {
            AuthRespReq req{};
            try
            {
                req = env->payload.get<AuthRespReq>();
            }
            catch (...)
            {
                return to_line(Envelope::error("usage", "auth_resp: user,response"));
            }

            if (!auth_->auth_complete(req.user, req.response)) return to_line(Envelope::error("auth_failed", "bad credentials"));

            authenticated = true;
            username = req.user;
            return to_line(Envelope::info("authentication successful"));
        }

        default:
            if (!authenticated) return to_line(Envelope::error("not_authenticated", "login first"));
            return to_line(Envelope::error("unknown_type", "not implemented"));
    }
}
