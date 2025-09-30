#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

class Session;
class Server;

class Room : public std::enable_shared_from_this<Room>
{
public:
    explicit Room(std::string name, const std::shared_ptr<Server>& server);

    std::string get_room_name() const { return room_name_; };
    std::vector<std::string> get_members() const;

    void join(const std::shared_ptr<Session>& session);
    void leave(const std::shared_ptr<Session>& session);
    void broadcast(const std::string& line);

    size_t size() const { return sessions_.size(); }

private:
    std::string room_name_;
    std::unordered_set<std::shared_ptr<Session>> sessions_;
    std::weak_ptr<Server> server_;
};