#include <chat/app/handlers/RoomHandlers.h>
#include <chat/protocol/JsonCodec.h>
#include <nlohmann/json.hpp>
using nlohmann::json;

std::string RoomHandlers::Join(const dto::rooms::Join& req, SessionCtx& ctx)
{
    if (req.room.empty()) return JsonCodec::to_line(Envelope::error("usage", "/join <room>"));
    svc_.Join(req.room, ctx.username, ctx.push);
    ctx.current_room = req.room;
    return JsonCodec::to_line(Envelope::info("joined " + req.room));
}
std::string RoomHandlers::Leave(SessionCtx& ctx)
{
    if (ctx.current_room.empty()) return JsonCodec::to_line(Envelope::error("not_in_room", "join a room first"));
    const auto room = ctx.current_room;
    svc_.Leave(ctx.username);
    ctx.current_room.clear();
    return JsonCodec::to_line(Envelope::info("left " + room));
}
std::string RoomHandlers::Rooms(SessionCtx&)
{
    auto list = svc_.Rooms();
    json arr = json::array();
    for (auto& [name, cnt] : list)
        arr.push_back({{"name", name}, {"members", cnt}});
    Envelope e;
    e.type = "rooms";
    e.payload = arr;
    return JsonCodec::to_line(e);
}
