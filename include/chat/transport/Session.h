#pragma once
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <chat/core/WriteQueueT.h>
#include <chat/protocol/SessionCtx.h>

class Server;

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace ssl = boost::asio::ssl;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket&& sock, std::shared_ptr<Server> srv);

    void start();

    void send(const std::string& s);

    void close();

private:
    void do_read_line();

    void on_read(const boost::system::error_code& ec, std::size_t n);

    void self_close_();

private:
    ssl::stream<tcp::socket> stream_;
    asio::streambuf buf_;
    std::weak_ptr<Server> server_;
    WriteQueueT<ssl::stream<tcp::socket>> writer_;
    SessionCtx ctx_;
};
