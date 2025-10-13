#include <chat/app/handlers/ChatHandlers.h>
#include <chat/protocol/JsonCodec.h>

std::string ChatHandlers::Send(const dto::chat::Msg& req, SessionCtx& ctx)
{
    if (ctx.current_room.empty()) return JsonCodec::to_line(Envelope::error("not_in_room", "join a room first"));
    if (req.text.empty()) return JsonCodec::to_line(Envelope::error("usage", "/msg <text>"));
    svc_.Broadcast(ctx.current_room, ctx.username, req.text);
    return JsonCodec::to_line(Envelope::info("sent"));
}
