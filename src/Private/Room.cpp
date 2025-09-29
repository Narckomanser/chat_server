#include "../Public/Room.h"

#include <iostream>

#include "../Public/Session.h"
#include "../Public/Server.h"
#include "../Public/Log.h"

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
    broadcast(":server INFO " + session->get_nick() + " joined " + room_name_ + "\n");
    log_line("INFO", "room", session->get_nick() + " joined room=" + room_name_);
}

void Room::leave(const std::shared_ptr<Session>& session)
{
    if (sessions_.erase(session))
    {
        broadcast(":server INFO " + session->get_nick() + " left " + room_name_ + "\n");
        log_line("INFO", "room", session->get_nick() + " left room=" + room_name_);

        server_.lock()->prune_empty_room(room_name_);
    }
}

void Room::broadcast(std::string line)
{
    for (const auto& session : sessions_)
    {
        session->deliver(line);
    }
}
