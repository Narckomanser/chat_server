#pragma once
#include <functional>
#include <string>
#include <chat/protocol/dto/Dto.h>

struct SessionCtx
{
    bool authenticated = false;
    std::string username;
    std::string current_room;
    std::function<void(const Envelope&)> push;
};
