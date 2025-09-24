#include "../Public/Server.h"

#include "iostream"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& context, uint16_t port) : acceptor_(tcp::acceptor(context, tcp::endpoint(tcp::v4(), port)))
{
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
}

void Server::start_accept()
{
    do_accept();
}

void Server::remove_session(const std::shared_ptr<Session>& session)
{
    if (auto it_to_remove = sessions_.find(session); it_to_remove != sessions_.end())
    sessions_.erase(session);
}

void Server::do_accept()
{
    auto socket = std::make_shared<tcp::socket>(acceptor_.get_executor());
    acceptor_.async_accept(*socket, std::bind(&Server::on_accept, this, socket, std::placeholders::_1));
}

void Server::on_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& ec)
{
    do_accept();

    if (ec)
    {
        std::cout << "[server] ERROR: " << ec.what() << std::endl;
        return;
    }

    auto session = std::make_shared<Session>(std::move(*socket), shared_from_this());
    sessions_.insert(session);
    session->start();

}
