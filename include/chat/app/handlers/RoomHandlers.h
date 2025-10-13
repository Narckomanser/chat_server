#pragma once
#include <chat/app/handlers/IRoomHandlers.h>
#include <chat/app/services/RoomService.h>

class RoomHandlers final : public IRoomHandlers
{
public:
    explicit RoomHandlers(RoomService& svc) : svc_(svc) {}

    std::string Join(const dto::rooms::Join& req, SessionCtx& ctx) override;
    std::string Leave(SessionCtx& ctx) override;
    std::string Rooms(SessionCtx& ctx) override;

private:
    RoomService& svc_;
};
