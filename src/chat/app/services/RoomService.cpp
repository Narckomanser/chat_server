#include <chat/app/services/RoomService.h>
#include <algorithm>

void RoomService::remove_if(std::vector<Member>& v, const std::string& user)
{
    v.erase(std::remove_if(v.begin(), v.end(), [&](const Member& m) { return m.nick == user; }), v.end());
}

void RoomService::Join(const std::string& room, const std::string& user, std::function<void(const Envelope&)> push)
{
    auto itPrev = user2room_.find(user);
    if (itPrev != user2room_.end() && itPrev->second != room)
    {
        Leave(user);
    }

    auto& members = rooms_[room];
    remove_if(members, user);
    members.push_back(Member{user, std::move(push)});
    user2room_[user] = room;

    Envelope ev;
    ev.type = "room_event";
    ev.payload = {{"kind", "join"}, {"room", room}, {"nick", user}};
    for (auto& m : members)
        m.push(ev);
}

void RoomService::Leave(const std::string& user)
{
    auto it = user2room_.find(user);
    if (it == user2room_.end()) return;

    const std::string room = it->second;
    auto rIt = rooms_.find(room);
    if (rIt == rooms_.end())
    {
        user2room_.erase(it);
        return;
    }

    auto& members = rIt->second;
    Envelope ev;
    ev.type = "room_event";
    ev.payload = {{"kind", "leave"}, {"room", room}, {"nick", user}};
    for (auto& m : members)
        m.push(ev);

    remove_if(members, user);
    user2room_.erase(it);

    if (members.empty()) rooms_.erase(rIt);
}

std::vector<std::pair<std::string, int>> RoomService::Rooms() const
{
    std::vector<std::pair<std::string, int>> out;
    out.reserve(rooms_.size());
    for (auto& [name, vec] : rooms_)
        out.emplace_back(name, (int)vec.size());
    return out;
}

void RoomService::Broadcast(const std::string& room, const std::string& from, const std::string& text)
{
    auto it = rooms_.find(room);
    if (it == rooms_.end()) return;
    Envelope ev;
    ev.type = "chat_event";
    ev.payload = {{"room", room}, {"from", from}, {"text", text}};
    for (auto& m : it->second)
        m.push(ev);
}

std::string RoomService::CurrentRoomOf(const std::string& user) const
{
    auto it = user2room_.find(user);
    return it == user2room_.end() ? std::string{} : it->second;
}
