#pragma once
#include <string>
#include <chat/protocol/SessionCtx.h>
#include <chat/protocol/dto/Dto.h>
#include <chat/protocol/dto/RoomDto.h>

class IRoomHandlers
{
public:
    virtual ~IRoomHandlers() = default;
    virtual std::string Join(const dto::rooms::Join& req, SessionCtx& ctx) = 0;
    virtual std::string Leave(SessionCtx& ctx) = 0;
    virtual std::string Rooms(SessionCtx& ctx) = 0;
};
