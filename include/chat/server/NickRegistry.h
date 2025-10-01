#pragma once

#include <unordered_map>
#include <memory>

#include <chat/core/Types.h>
#include <chat/server/NickHash.h>

class Session;

class NickRegistry
{
public:
    bool set(const std::shared_ptr<Session>& who, const Nick& nick, std::string& reason);
    void drop(const Nick& nickname) { registry_.erase(nickname); }
    std::shared_ptr<Session> find(const std::string_view& nick_sv);
    std::shared_ptr<Session> find(const Nick& nick);

private:
    std::unordered_map<Nick, std::weak_ptr<Session>, NickHash, NickEq> registry_;
};