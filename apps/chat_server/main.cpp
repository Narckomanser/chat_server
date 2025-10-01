#include "../../include/chat/server/Server.h"
#include <boost/asio.hpp>
#include <iostream>

#include "../../include/chat/core/Log.h"

namespace asio = boost::asio;

static unsigned short DEFAULT_PORT = 5555;

int main(int argc, char** argv)
{
    try
    {
        unsigned short port = DEFAULT_PORT;

        if (const char* p = std::getenv("PORT")) {
            port = static_cast<unsigned short>(std::stoi(p));
        }
        for (int i = 1; i + 1 < argc; ++i) {
            if (std::strcmp(argv[i], "--port") == 0) {
                port = static_cast<unsigned short>(std::stoi(argv[i+1]));
            }
        }

        asio::io_context io;
        auto server = std::make_shared<Server>(io, port);
        server->start_accept();

        log_line("INFO", "server", "listening port= " + std::to_string(port));
        io.run();
    }
    catch (const std::exception& e)
    {
        log_line("ERROR", "fatal", e.what());
        return 1;
    }
    return 0;
}
