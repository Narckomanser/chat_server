#include <chat/protocol/JsonCodec.h>

#include <chat/protocol/MessageTypes.h>

using json = nlohmann::json;

std::string JsonCodec::to_line(const Envelope& envelope)
{
    json j;

    j["type"] = std::string(to_string(envelope.type));
    if (envelope.req_id) j["req_id"] = *envelope.req_id;
    if (envelope.ts) j["ts"] = envelope.ts;
    j["payload"] = envelope.payload;

    std::string s = j.dump();
    s.push_back('\n');

    return s;
}

std::optional<Envelope> JsonCodec::from_line(std::string_view line)
{
    try
    {
        auto j = json::parse(line);
        Envelope envelope;

        envelope.type = msg_type_from(j.at("type").get<std::string>());
        if (auto it = j.find("req_id"); it != j.end() && it->is_string()) envelope.req_id = it->get<std::string>();
        if (auto it = j.find("ts"); it != j.end() && it->is_number_integer()) envelope.ts = it->get<int64_t>();
        envelope.payload = j.value("payload", json::object());

        return envelope;
    }
    catch (...)
    {
        return std::nullopt;
    }
}
