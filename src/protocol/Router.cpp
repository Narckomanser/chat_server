#include <chat/protocol/Router.h>
#include <chat/protocol/JsonCodec.h>
#include <chat/protocol/dto/Dto.h>
#include <chat/protocol/dto/AuthDto.h>
#include <chat/protocol/dto/RoomDto.h>
#include <chat/protocol/dto/ChatDto.h>
#include <chat/app/handlers/IAuthHandlers.h>
#include <chat/app/handlers/IRoomHandlers.h>
#include <chat/app/handlers/IChatHandlers.h>
#include <nlohmann/json.hpp>
using nlohmann::json;

static inline std::string to_line(const Envelope& e)
{
    return JsonCodec::to_line(e);
}

std::string Router::Handle(const std::string& json_line, SessionCtx& ctx)
{
    auto env = JsonCodec::from_line(json_line);
    if (!env) return to_line(Envelope::error("bad_json", "cannot parse"));

    const std::string& t = env->type;

    if (t == "quit") return to_line(Envelope::info("bye"));

    if (t == "auth")
    {
        dto::auth::Auth req{};
        try
        {
            req = env->payload.get<dto::auth::Auth>();
        }
        catch (...)
        {
            return to_line(Envelope::error("usage", "auth: need user"));
        }
        return auth_->Auth(req, ctx);
    }
    if (t == "auth_resp")
    {
        dto::auth::AuthResp req{};
        try
        {
            req = env->payload.get<dto::auth::AuthResp>();
        }
        catch (...)
        {
            return to_line(Envelope::error("usage", "auth_resp: user,response"));
        }
        return auth_->AuthResp(req, ctx);
    }

    if (!ctx.authenticated) return to_line(Envelope::error("not_authenticated", "login first"));

    if (t == "join")
    {
        dto::rooms::Join req{};
        try
        {
            req = env->payload.get<dto::rooms::Join>();
        }
        catch (...)
        {
            return to_line(Envelope::error("usage", "join: room"));
        }
        return rooms_->Join(req, ctx);
    }
    if (t == "leave")
    {
        return rooms_->Leave(ctx);
    }
    if (t == "rooms")
    {
        return rooms_->Rooms(ctx);
    }

    if (t == "msg")
    {
        dto::chat::Msg req{};
        try
        {
            req = env->payload.get<dto::chat::Msg>();
        }
        catch (...)
        {
            return to_line(Envelope::error("usage", "msg: text"));
        }
        return chat_->Send(req, ctx);
    }

    return to_line(Envelope::error("unknown_type", "not implemented"));
}
