#include "Public/Server.h"
#include <boost/asio.hpp>
#include <iostream>

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

        std::cout << "[INFO] server listening port=" << port << std::endl;
        io.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ERROR] fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
