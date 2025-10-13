#include <chat/protocol/JsonCodec.h>

using json = nlohmann::json;

std::string JsonCodec::to_line(const Envelope& e)
{
    json j;
    j["type"] = e.type;
    if (e.req_id) j["req_id"] = *e.req_id;
    if (e.ts) j["ts"] = e.ts;
    j["payload"] = e.payload;
    std::string s = j.dump();
    s.push_back('\n');
    return s;
}

std::optional<Envelope> JsonCodec::from_line(std::string_view line)
{
    try
    {
        auto j = json::parse(line);
        Envelope e;
        e.type = j.at("type").get<std::string>();
        if (auto it = j.find("req_id"); it != j.end() && it->is_string()) e.req_id = it->get<std::string>();
        if (auto it = j.find("ts"); it != j.end() && it->is_number_integer()) e.ts = it->get<int64_t>();
        e.payload = j.value("payload", json::object());
        return e;
    }
    catch (...)
    {
        return std::nullopt;
    }
}
