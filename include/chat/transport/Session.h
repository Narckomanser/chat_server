#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <chat/core/WriteQueueT.h>

#include <memory>
#include <string>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace ssl = asio::ssl;


class Server;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket&& socket, std::shared_ptr<Server> server);

    void start();
    void close();

private:
    void do_read_line();
    void on_read(boost::system::error_code& ec, size_t);
    void send(const std::string& line);

private:
    ssl::stream<tcp::socket> stream_;
    asio::streambuf buf_;
    std::weak_ptr<Server> server_;
    WriteQueueT<ssl::stream<tcp::socket>> writer_;
    bool authenticated_{false};
    std::string username_;
};