#include <chat/core/Log.h>
#include <chat/transport/Server.h>
#include <chat/protocol/Router.h>
#include <chat/infra/db/sqlite/UserRepository.h>
#include <chat/infra/security/NonceStore.h>
#include <chat/app/services/AuthService.h>

#include <boost/asio.hpp>
#include <cstring>
#include <memory>

namespace asio = boost::asio;

static unsigned short DEFAULT_PORT = 5555;

int main(int argc, char** argv)
{
    unsigned short port = DEFAULT_PORT;
    std::string tls_cert = "chat.crt";
    std::string tls_key = "chat.key";
    std::string realm = "accused_chat";
    int nonce_ttl_sec = 60;

    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--port") == 0 && i + 1 < argc)
            port = static_cast<unsigned short>(std::stoi(argv[++i]));
        else if (std::strcmp(argv[i], "--tls-cert") == 0 && i + 1 < argc)
            tls_cert = argv[++i];
        else if (std::strcmp(argv[i], "--tls-key") == 0 && i + 1 < argc)
            tls_key = argv[++i];
        else if (std::strcmp(argv[i], "--realm") == 0 && i + 1 < argc)
            realm = argv[++i];
        else if (std::strcmp(argv[i], "--nonce-ttl") == 0 && i + 1 < argc)
            nonce_ttl_sec = std::stoi(argv[++i]);
    }

    try
    {
        asio::io_context io;

        auto users = std::make_shared<UserRepository>("chat.db");
        auto nonces = std::make_shared<NonceStore>(nonce_ttl_sec);
        auto auth_service = std::make_shared<AuthService>(*users, realm, *nonces);
        auto router = std::make_shared<Router>(auth_service);
        auto server = std::make_shared<Server>(io, port, tls_cert, tls_key, router);

        server->start();

        log_line("INFO", "server", "listening port=" + std::to_string(port) + " realm=" + realm);
        io.run();
    }
    catch (const std::exception& e)
    {
        log_line("ERROR", "fatal", e.what());
        return 1;
    }

    return 0;
}
