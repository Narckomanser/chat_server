#pragma once

#include "boost/asio.hpp"

#include "deque"
#include "string"
#include "memory"

#include "Server.h"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket&& socket, std::shared_ptr<Server> server);
    void start();
    void deliver(std::string line);
    std::string get_remote_ip() const;

private:
    void do_read_line();
    void on_read(const boost::system::error_code& ec, std::size_t size);
    void do_write();
    void on_write(const boost::system::error_code& ec, std::size_t size);
    void close();

private:
    tcp::socket socket_;
    asio::streambuf read_buf_;
    std::weak_ptr<Server> server_;
    std::deque<std::string> outbox_;
};
