#pragma once

#include <deque>
#include <string>
#include <functional>
#include <boost/asio.hpp>

template <typename Stream>
class WriteQueueT
{
public:
    explicit WriteQueueT(Stream& stream) : stream_(stream) {}

    void enqueue(std::string s)
    {
        queue_.push_back(std::move(s));
        if (queue_.size() == 1) kick();
    }

private:
    void kick()
    {
        auto& front = queue_.front();
        boost::asio::async_write(        //
            stream_,                     //
            boost::asio::buffer(front),  //
            [this](const boost::system::error_code& ec, std::size_t) { on_write(ec); });
    }

    void on_write(const boost::system::error_code ec)
    {
        if (ec)
        {
            if (on_error_)
            {
                on_error_(ec);
                return;
            }
        }
        queue_.pop_front();
        if (!queue_.empty()) kick();
    }

public:
    std::function<void(const boost::system::error_code&)> on_error_;

private:
    Stream& stream_;
    std::deque<std::string> queue_;
};