#include "../Public/Room.h"

#include "../Public/Session.h"

Room::Room(std::string name) : room_name_(std::move(name)) {}

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

void Room::join(std::shared_ptr<Session>& session)
{
    sessions_.insert(session);
    broadcast(":server INFO " + session->get_nick() + " joined " + room_name_ + "\n");
}

void Room::leave(std::shared_ptr<Session>& session)
{
    if (sessions_.erase(session))
    {
        broadcast(":server INFO " + session->get_nick() + " left " + room_name_ + "\n");
    }
}

void Room::broadcast(std::string& line)
{
    for (const auto& session : sessions_)
    {
        session->deliver(line);
    }
}
