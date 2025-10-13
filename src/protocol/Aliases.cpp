#include <chat/protocol/Aliases.h>
#include <chat/protocol/dto/Dto.h>
#include <chat/protocol/JsonCodec.h>
#include <sstream>

static std::string trim(std::string s) {
    while (!s.empty() && (s.back()=='\r' || s.back()=='\n' || s.back()==' ' || s.back()=='\t')) s.pop_back();
    return s;
}

std::optional<std::string> alias_to_json_line(std::string_view sv) {
    if (sv.empty() || sv.front() != '/') return std::nullopt;

    std::string line(sv);
    line = trim(line);
    std::istringstream iss(line.substr(1));

    std::string cmd; iss >> cmd;
    Envelope e;

    if (cmd == "auth") {
        std::string user; iss >> user;
        if (user.empty()) return JsonCodec::to_line(Envelope::error("usage", "usage: /auth <user>"));
        e.type = "auth";
        e.payload = {{"user", user}};
        return JsonCodec::to_line(e);
    }
    if (cmd == "auth_resp") {
        std::string user, response; iss >> user >> response;
        if (user.empty() || response.empty())
            return JsonCodec::to_line(Envelope::error("usage", "usage: /auth_resp <user> <response_hex>"));
        e.type = "auth_resp";
        e.payload = {{"user", user}, {"response", response}};
        return JsonCodec::to_line(e);
    }
    if (cmd == "join") {
        std::string room; iss >> room;
        if (room.empty()) return JsonCodec::to_line(Envelope::error("usage", "usage: /join <room>"));
        e.type = "join";
        e.payload = {{"room", room}};
        return JsonCodec::to_line(e);
    }
    if (cmd == "leave") {
        e.type = "leave";
        e.payload = nlohmann::json::object();
        return JsonCodec::to_line(e);
    }
    if (cmd == "rooms") {
        e.type = "rooms";
        e.payload = nlohmann::json::object();
        return JsonCodec::to_line(e);
    }
    if (cmd == "msg") {
        std::string text;
        std::getline(iss, text);
        if (!text.empty() && text.front()==' ') text.erase(0,1);
        if (text.empty()) return JsonCodec::to_line(Envelope::error("usage", "usage: /msg <text>"));
        e.type = "msg";
        e.payload = {{"text", text}};
        return JsonCodec::to_line(e);
    }
    if (cmd == "quit") {
        e.type = "quit";
        e.payload = nlohmann::json::object();
        return JsonCodec::to_line(e);
    }

    return JsonCodec::to_line(Envelope::error("unknown_alias", "unknown alias"));
}
