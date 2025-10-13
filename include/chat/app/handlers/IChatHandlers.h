#pragma once
#include <string>
#include <chat/protocol/SessionCtx.h>
#include <chat/protocol/dto/Dto.h>
#include <chat/protocol/dto/ChatDto.h>

class IChatHandlers
{
public:
    virtual ~IChatHandlers() = default;
    virtual std::string Send(const dto::chat::Msg& req, SessionCtx& ctx) = 0;
};
