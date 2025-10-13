#pragma once
#include <chat/app/handlers/IChatHandlers.h>
#include <chat/app/services/RoomService.h>
#include <chat/protocol/dto/ChatDto.h>

class ChatHandlers final : public IChatHandlers
{
public:
    explicit ChatHandlers(RoomService& svc) : svc_(svc) {}

    std::string Send(const dto::chat::Msg& req, SessionCtx& ctx) override;

private:
    RoomService& svc_;
};
