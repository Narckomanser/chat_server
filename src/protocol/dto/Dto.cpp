#include <chat/protocol/dto/Dto.h>
#include <chrono>

static int64_t now_sec()
{
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

Envelope Envelope::info(std::string text)
{
    Envelope e;
    e.type = "info";
    e.payload = json{{"text", std::move(text)}};
    e.ts = now_sec();
    return e;
}

Envelope Envelope::error(std::string code, std::string message)
{
    Envelope e;
    e.type = "error";
    e.payload = json{{"code", std::move(code)}, {"message", std::move(message)}};
    e.ts = now_sec();
    return e;
}

Envelope Envelope::hello(std::vector<std::string> caps)
{
    Envelope e;
    e.type = "hello";
    e.payload = json{{"proto", 2}, {"cap", std::move(caps)}};
    e.ts = now_sec();
    return e;
}

Envelope Envelope::auth_challenge(std::string realm, std::string nonce, int ttl)
{
    Envelope e;
    e.type = "auth_challenge";
    e.payload = json{{"realm", std::move(realm)}, {"nonce", std::move(nonce)}, {"ttl", ttl}};
    e.ts = now_sec();
    return e;
}
