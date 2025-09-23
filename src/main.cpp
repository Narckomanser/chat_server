#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

static const unsigned short DEFAULT_PORT = 5555;

int main()
{
    try
    {
        //TODO should read from property file and check port accessibility before use or DEFAULT_PORT
        const unsigned short PORT = DEFAULT_PORT;

        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));

        std::cout << "[server] listening on " << PORT << std::endl;

        for (;;) {
            tcp::socket sock(io_context);
            acceptor.accept(sock);
            std::cout << "[server] client connected from "
                      << sock.remote_endpoint().address().to_string() << std::endl;
            boost::asio::write(sock, boost::asio::buffer("hello from asio\n"));
        }
    }
        catch(const std::exception& e)
        {
            std::cerr << "[server] ERROR: " << e.what() << std::endl;
            return 1;
        }
    }
