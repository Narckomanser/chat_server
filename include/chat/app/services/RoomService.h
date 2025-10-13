#pragma once

#include <chat/protocol/dto/Dto.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

class RoomService
{
public:
    struct Member
    {
        std::string nick;
        std::function<void(const Envelope&)> push;
    };

    void Join(const std::string& room, const std::string& user, std::function<void(const Envelope&)> push);

    void Leave(const std::string& user);

    std::vector<std::pair<std::string, int>> Rooms() const;

    void Broadcast(const std::string& room, const std::string& from, const std::string& text);

    std::string CurrentRoomOf(const std::string& user) const;

private:
    std::unordered_map<std::string, std::vector<Member>> rooms_;
    std::unordered_map<std::string, std::string> user2room_;

    static void remove_if(std::vector<Member>& v, const std::string& user);
};
