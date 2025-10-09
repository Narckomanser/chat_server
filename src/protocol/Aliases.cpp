#include <chat/protocol/Aliases.h>

#include <sstream>

std::optional<std::string> alias_to_json(std::string_view line)
{
    if (!line.empty() || line[0] != '/') return std::nullopt;

    std::istringstream iss(std::string(line.substr(1)));
    std::string cmd;
    iss >> cmd;

    if (cmd == "auth")
    {
        std::string user;
        iss >> user;
        if (user.empty()) return std::string("{\"type\":\"error\",\"payload\":{\"code\":\"usage\",\"message\":\"/auth <user>\"}}\n");
        return std::string("{\"type\":\"auth\",\"payload\":{\"user\":\"") + user + "\"}}\n";
    }
    else if (cmd == "auth_resp")
    {
        std::string user, resp;
        iss >> user >> resp;
        if (user.empty() || resp.empty())
            return std::string("{\"type\":\"error\",\"payload\":{\"code\":\"usage\",\"message\":\"/auth_resp <user> <response_hex>\"}}\n");
        return std::string("{\"type\":\"auth_resp\",\"payload\":{\"user\":\"") + user + "\",\"response\":\"" + resp + "\"}}\n";
    }
    else if (cmd == "quit")
    {
        return std::string("{\"type\":\"quit\"}\n");
    }
    return std::string("{\"type\":\"error\",\"payload\":{\"code\":\"unknown_command\",\"message\":\"unknown alias\"}}\n");
}
