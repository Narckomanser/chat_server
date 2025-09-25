#pragma once

#include "boost/asio.hpp"

#include "unordered_set"
#include "memory"

class Session;

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Server : public std::enable_shared_from_this<Server>
{
public:
    Server(asio::io_context& context, uint16_t port);
    void start_accept();
    void remove_session(const std::shared_ptr<Session>& session);

private:
    void do_accept();
    void on_accept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& ec);


private:
    tcp::acceptor acceptor_;
    std::unordered_set<std::shared_ptr<Session>> sessions_;
};