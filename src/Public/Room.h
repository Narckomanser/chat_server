#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

class Session;

class Room : public std::enable_shared_from_this<Room>
{
public:
    Room(std::string name);

    const std::string get_room_name() const { return room_name_; };
    std::vector<std::string> get_members() const;

    void join(std::shared_ptr<Session>& sesson);
    void leave(std::shared_ptr<Session>& session);
    void broadcast(std::string& line);

private:
    std::string room_name_;
    std::unordered_set<std::shared_ptr<Session>> sessions_;
};