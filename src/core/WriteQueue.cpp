#include "../../include/chat/core/WriteQueue.h"

void WriteQueue::enqueue(std::string s)
{
    queue_.push_back(std::move(s));
    if (queue_.size() == 1) kick();
}

void WriteQueue::kick()
{
    auto& front = queue_.front();
    boost::asio::async_write(                   //
        socket_,                             //
        boost::asio::buffer(front),   //
        [this](const boost::system::error_code& ec, std::size_t) { this->on_write(ec); });
}

void WriteQueue::on_write(const boost::system::error_code& ec)
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