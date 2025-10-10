#include <chat/protocol/Dto.h>

#include <chrono>

static int64_t now_sec()
{
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

Envelope Envelope::info(std::string text)
{
    Envelope envelope;
    envelope.type = MsgType::Info;
    envelope.payload = InfoPayload{std::move(text)};
    envelope.ts = now_sec();

    return envelope;
}

Envelope Envelope::error(std::string code, std::string message)
{
    Envelope envelope;
    envelope.type = MsgType::Error;
    envelope.payload = ErrorPayload{std::move(code), std::move(message)};
    envelope.ts = now_sec();

    return envelope;
}

Envelope Envelope::hello(std::vector<std::string> caps)
{
    Envelope envelope;
    envelope.type = MsgType::Hello;
    envelope.payload = json{{"proto", 2}, {"cap", caps}};
    envelope.ts = now_sec();

    return envelope;
}

Envelope Envelope::auth_challenge(std::string realm, std::string nonce, int ttl)
{
    Envelope envelope;
    envelope.type = MsgType::Info;
    envelope.payload = AuthChallengePayload{std::move(realm), std::move(nonce), ttl};
    envelope.ts = now_sec();

    return envelope;
}