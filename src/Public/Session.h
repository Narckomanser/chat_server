#pragma once

#include "boost/asio.hpp"

#include <deque>
#include <string>
#include <memory>
#include <chrono>

#include "../Public/Message.h"
#include "../Public/WriteQueue.h"
#include "../Public/RateLimiter.h"
#include "../Public/Types.h"

class Server;
class Room;

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket&& socket, const std::shared_ptr<Server>& server);
    void start();
    void deliver(std::string line);
    std::string get_remote_ip() const;
    const Nick& get_nick() const { return nick_; }
    void set_room(const std::shared_ptr<Room>& room);
    std::string get_room_name() const;

private:
    void do_read_line();
    void on_read(const boost::system::error_code& ec);
    void close();
    void handle_command(const std::string& line);
    void send_info(const std::string& text) { deliver(format_info(text)); }
    void send_error(const std::string& text) {deliver(format_error(text)); }

private:
    tcp::socket socket_;
    asio::streambuf read_buf_;
    std::weak_ptr<Server> server_;
    WriteQueue writer_;
    Nick nick_;
    std::weak_ptr<Room> room_;
    RateLimiter rate_;

};
