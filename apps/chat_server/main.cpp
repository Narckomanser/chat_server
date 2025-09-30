#include "../../include/chat/server/Server.h"
#include <boost/asio.hpp>
#include <iostream>

#include "../../include/chat/core/Log.h"

namespace asio = boost::asio;

static unsigned short DEFAULT_PORT = 5555;

int main()
{
    try
    {
        unsigned short port = DEFAULT_PORT;
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
