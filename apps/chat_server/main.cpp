#include <chat/core/Log.h>

#include <chat/infra/db/sqlite/SqliteDatabase.h>
#include <chat/infra/db/dao/SqliteUserDao.h>
#include <chat/infra/repos/UserRepository.h>

#include <chat/infra/security/NonceStore.h>

#include <chat/app/services/AuthService.h>
#include <chat/app/services/RoomService.h>

#include <chat/app/handlers/IAuthHandlers.h>
#include <chat/app/handlers/IRoomHandlers.h>
#include <chat/app/handlers/IChatHandlers.h>
#include <chat/app/handlers/AuthHandlers.h>
#include <chat/app/handlers/RoomHandlers.h>
#include <chat/app/handlers/ChatHandlers.h>

#include <chat/protocol/Router.h>
#include <chat/transport/Server.h>

#include <boost/asio.hpp>
#include <cstring>
#include <memory>
#include <string>

namespace asio = boost::asio;

static void print_usage(const char* argv0) {
    std::string exe = argv0 ? argv0 : "chat_server";
    std::fprintf(stderr,
        "Usage: %s [--port PORT] [--tls-cert FILE] [--tls-key FILE] [--realm STR] [--nonce-ttl SECONDS] [--db FILE]\n"
        "Defaults:\n"
        "  --port       5555\n"
        "  --tls-cert   chat.crt\n"
        "  --tls-key    chat.key\n"
        "  --realm      accused_chat\n"
        "  --nonce-ttl  60\n"
        "  --db         chat.db\n", exe.c_str());
}

int main(int argc, char** argv) {
    unsigned short port = 5555;
    std::string tls_cert = "chat.crt";
    std::string tls_key  = "chat.key";
    std::string realm    = "accused_chat";
    int nonce_ttl_sec    = 60;
    std::string db_file  = "chat.db";

    for (int i = 1; i < argc; ++i) {
        if (!std::strcmp(argv[i], "--help") || !std::strcmp(argv[i], "-h")) {
            print_usage(argv[0]);
            return 0;
        } else if (!std::strcmp(argv[i], "--port") && i + 1 < argc) {
            port = static_cast<unsigned short>(std::stoi(argv[++i]));
        } else if (!std::strcmp(argv[i], "--tls-cert") && i + 1 < argc) {
            tls_cert = argv[++i];
        } else if (!std::strcmp(argv[i], "--tls-key") && i + 1 < argc) {
            tls_key = argv[++i];
        } else if (!std::strcmp(argv[i], "--realm") && i + 1 < argc) {
            realm = argv[++i];
        } else if (!std::strcmp(argv[i], "--nonce-ttl") && i + 1 < argc) {
            nonce_ttl_sec = std::stoi(argv[++i]);
        } else if (!std::strcmp(argv[i], "--db") && i + 1 < argc) {
            db_file = argv[++i];
        } else {
            std::fprintf(stderr, "Unknown/invalid arg: %s\n", argv[i]);
            print_usage(argv[0]);
            return 2;
        }
    }

    try {
        asio::io_context io;

        // ----------------------- Database init
        SqliteConfig dbCfg;
        dbCfg.enable_foreign_keys = true;
        dbCfg.wal_journal        = true;
        dbCfg.busy_timeout_ms    = 3000;

        auto db = SqliteDatabase::open(db_file, dbCfg);

        auto userDao = std::make_shared<SqliteUserDao>(db);
        userDao->ensureSchema(); // создаст таблицу users при первом запуске

        UserRepository usersRepo(*userDao);

        // ----------------------- Security stores / services
        auto nonces = std::make_shared<NonceStore>(nonce_ttl_sec);
        AuthService authSvc(usersRepo, realm, *nonces);
        RoomService roomSvc;

        // ----------------------- Handlers
        auto authHandlers = std::make_shared<AuthHandlers>(authSvc);
        auto roomHandlers = std::make_shared<RoomHandlers>(roomSvc);
        auto chatHandlers = std::make_shared<ChatHandlers>(roomSvc);

        // ----------------------- Router
        auto router = std::make_shared<Router>(authHandlers, roomHandlers, chatHandlers, realm);

        // ----------------------- Server (TLS)
        auto server = std::make_shared<Server>(io, port, tls_cert, tls_key, router);
        server->start();

        log_line("INFO","server",
            "listening port=" + std::to_string(port) +
                " realm=" + realm +
                " db=" + db_file +
                " tls_cert=" + tls_cert +
                " tls_key=" + tls_key);

        io.run();
    } catch (const std::exception& e) {
        log_line("ERROR","fatal", e.what());
        return 1;
    }
    return 0;
}
