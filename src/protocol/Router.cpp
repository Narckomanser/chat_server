#include <chat/protocol/Router.h>

#include "chat/app/services/AuthService.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static std::string jerr(const std::string& code, const std::string& msg)
{
    json j;
    j["type"] = "error";
    j["payload"] = {{"code", code}, {"message", msg}};
    return j.dump() + "\n";
}
static std::string jinfo(const std::string& text)
{
    json j;
    j["type"] = "info";
    j["payload"] = {{"text", text}};
    return j.dump() + "\n";
}
static std::string auth_chal(const std::string& realm, const std::string& nonce, int ttl)
{
    json j;
    j["type"] = "auth_challenge";
    j["payload"] = {{"realm", realm}, {"nonce", nonce}, {"ttl", ttl}};
    return j.dump() + "\n";
}

Router::Router(std::shared_ptr<AuthService> auth) : auth_(auth) {}

std::string Router::handle(const std::string& json_line, bool& authenticated, std::string& username)
{
    json in;
    try
    {
        in = json::parse(json_line);
    }
    catch (...)
    {
        return jerr("bad_json", "cannot parse");
    }

    const auto type_it = in.find("type");
    if (type_it == in.end() || !type_it->is_string()) return jerr("bad_request", "missing type");
    const std::string type = *type_it;

    if (type == "quit")
    {
        return jinfo("bye");
    }

    if (type == "auth")
    {
        auto& pl = in["payload"];
        if (!pl.contains("user")) return jerr("usage", "auth: need user");
        std::string user = pl["user"];
        auto nonce_opt = auth_->auth_challenge(user);
        if (!nonce_opt) return jerr("unknown_user", "user not found");
        username = user;
        return auth_chal(auth_->realm(), *nonce_opt, 60);
    }

    if (type == "auth_resp")
    {
        auto& pl = in["payload"];
        if (!pl.contains("user") || !pl.contains("response")) return jerr("usage", "auth_resp: need user,response");
        std::string user = pl["user"];
        std::string resp = pl["response"];
        if (!auth_->auth_complete(user, resp)) return jerr("auth_failed", "bad credentials");
        authenticated = true;
        username = user;
        return jinfo("authentication successful");
    }

    // неавторизованные команды — резать здесь (позже whitelist расширим)
    if (!authenticated) return jerr("not_authenticated", "login first");

    // сюда добавим join/msg/pm/rooms в следующих шагах
    return jerr("unknown_type", "not implemented");
}
