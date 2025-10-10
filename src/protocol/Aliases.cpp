#include <chat/protocol/Aliases.h>

#include <chat/protocol/Dto.h>
#include <chat/protocol/JsonCodec.h>
#include <sstream>

static std::string trim(std::string s)
{
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t'))
        s.pop_back();
    return s;
}

std::optional<std::string> alias_to_json_line(std::string_view sv)
{
    if (sv.empty() || sv.front() != '/') return std::nullopt;
    std::string line(sv);
    line = trim(line);
    std::istringstream iss(line.substr(1));

    std::string cmd;
    iss >> cmd;

    Envelope env;

    if (cmd == "auth")
    {
        std::string user;
        iss >> user;
        if (user.empty()) return JsonCodec::to_line(Envelope::error("usage", "usage: /auth <user>"));
        env.type = MsgType::Auth;
        env.payload = nlohmann::json{{"user", user}};
        return JsonCodec::to_line(env);
    }
    else if (cmd == "auth_resp")
    {
        std::string user, response;
        iss >> user >> response;
        if (user.empty() || response.empty())
            return JsonCodec::to_line(Envelope::error("usage", "usage: /auth_resp <user> <response_hex>"));
        env.type = MsgType::AuthResp;
        env.payload = nlohmann::json{{"user", user}, {"response", response}};
        return JsonCodec::to_line(env);
    }
    else if (cmd == "quit")
    {
        env.type = MsgType::Quit;
        env.payload = nlohmann::json::object();
        return JsonCodec::to_line(env);
    }

    return JsonCodec::to_line(Envelope::error("unknown_command", "unknown alias"));
}
