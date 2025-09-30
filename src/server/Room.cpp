#include "../../include/chat/server/Room.h"

#include "../../include/chat/server/Session.h"
#include "../../include/chat/server/Server.h"
#include "../../include/chat/core/Log.h"

Room::Room(std::string name, const std::shared_ptr<Server>& server) : room_name_(std::move(name)), server_(server) {}

std::vector<std::string> Room::get_members() const
{
    std::vector<std::string> members;
    members.reserve(sessions_.size());
    for (const auto& session : sessions_)
    {
        if (session) members.push_back(session->get_nick());
    }

    return members;
}

void Room::join(const std::shared_ptr<Session>& session)
{
    sessions_.insert(session);
    broadcast(format_info(session->get_nick() + " joined " + room_name_));
    log_line("INFO", "room", session->get_nick() + " joined room=" + room_name_);
}

void Room::leave(const std::shared_ptr<Session>& session)
{
    if (sessions_.erase(session))
    {
        broadcast(format_info(session->get_nick() + " left " + room_name_));
        log_line("INFO", "room", session->get_nick() + " left room=" + room_name_);

        server_.lock()->prune_empty_room(room_name_);
    }
}

void Room::broadcast(const std::string& line)
{
    for (const auto& session : sessions_)
    {
        session->deliver(line);
    }
}
