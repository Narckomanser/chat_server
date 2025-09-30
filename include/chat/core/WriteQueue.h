#pragma once

#include <deque>
#include <string>
#include <functional>
#include <boost/asio.hpp>

class WriteQueue
{
public:
    explicit WriteQueue(boost::asio::ip::tcp::socket& socket) : socket_(socket) {}

    void enqueue(std::string s);

private:
    void kick();
    void on_write(const boost::system::error_code& ec);

public:
    std::function<void(const boost::system::error_code&)> on_error_;

private:
    boost::asio::ip::tcp::socket& socket_;
    std::deque<std::string> queue_;
};