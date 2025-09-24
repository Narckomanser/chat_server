#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

static const unsigned short DEFAULT_PORT = 5555;

int main()
{
    try
    {
        // TODO should read from property file and check port accessibility before use or DEFAULT_PORT
        const unsigned short PORT = DEFAULT_PORT;

        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));

        std::cout << "[server] listening on " << PORT << std::endl;

        auto socket(std::make_shared<tcp::socket>(io_context));

        acceptor.async_accept(*socket,
            [socket, &acceptor](const boost::system::error_code& errorCode)
            {
                if (errorCode)
                {
                    std::cout << "[acceptor] ERROR: " << errorCode.what() << std::endl;
                    acceptor.async_accept(*socket);
                    return;
                }

                std::cout << "[server] client connected from " << socket->remote_endpoint().address().to_string() << std::endl;

                boost::asio::async_write(*socket, boost::asio::buffer("hello from asio\n"),
                    [socket](const boost::system::error_code& error, std::size_t bytes_transferred) {});


                auto next_socket(std::make_shared<tcp::socket>(io_context));
                acceptor.async_accept(*next_socket, );
            });

        io_context.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[server] ERROR: " << e.what() << std::endl;
        return 1;
    }
}