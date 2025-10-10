#pragma once

#include <string>
#include <string_view>
#include <optional>

enum class MsgType
{
    Hello,
    Info,
    Error,
    Quit,
    Auth,
    AuthChallenge,
    AuthResp,
    RegisterRequest,
    RegisterResp,
    Join,
    Leave,
    Rooms,
    Msg,
    Pm,
    Event,
    Unknown
};

inline std::string_view to_string(MsgType type)
{
    switch (type)
    {
        case MsgType::Hello: return "hello";
        case MsgType::Info: return "info";
        case MsgType::Error: return "error";
        case MsgType::Quit: return "quit";
        case MsgType::Auth: return "auth";
        case MsgType::AuthChallenge: return "auth_challenge";
        case MsgType::AuthResp: return "auth_resp";
        case MsgType::RegisterRequest: return "register_request";
        case MsgType::RegisterResp: return "register_resp";
        case MsgType::Join: return "join";
        case MsgType::Leave: return "leave";
        case MsgType::Rooms: return "rooms";
        case MsgType::Msg: return "msg";
        case MsgType::Pm: return "pm";
        case MsgType::Event: return "event";
        default: return "unknown";
    }
}

inline MsgType msg_type_from(std::string_view s)
{
#define MAP(x) if (s == #x) return MsgType::x
    if (s == "hello") return MsgType::Hello;
    if (s == "info") return MsgType::Info;
    if (s == "error") return MsgType::Error;
    if (s == "quit") return MsgType::Quit;
    if (s == "auth") return MsgType::Auth;
    if (s == "auth_challenge") return MsgType::AuthChallenge;
    if (s == "auth_resp") return MsgType::AuthResp;
    if (s == "register_request") return MsgType::RegisterRequest;
    if (s == "register_resp") return MsgType::RegisterResp;
    if (s == "join") return MsgType::Join;
    if (s == "leave") return MsgType::Leave;
    if (s == "rooms") return MsgType::Rooms;
    if (s == "msg") return MsgType::Msg;
    if (s == "pm") return MsgType::Pm;
    if (s == "event") return MsgType::Event;
    return MsgType::Unknown;
}