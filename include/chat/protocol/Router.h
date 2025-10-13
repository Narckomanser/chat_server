#pragma once
#include <memory>
#include <string>
#include <chat/protocol/SessionCtx.h>

class IAuthHandlers;
class IRoomHandlers;
class IChatHandlers;

class Router
{
public:
    Router(std::shared_ptr<IAuthHandlers> ah, std::shared_ptr<IRoomHandlers> rh, std::shared_ptr<IChatHandlers> ch, std::string realm)
        : auth_(std::move(ah)), rooms_(std::move(rh)), chat_(std::move(ch)), realm_(std::move(realm))
    {
    }

    std::string Handle(const std::string& json_line, SessionCtx& ctx);

private:
    std::shared_ptr<IAuthHandlers> auth_;
    std::shared_ptr<IRoomHandlers> rooms_;
    std::shared_ptr<IChatHandlers> chat_;
    std::string realm_;
};
