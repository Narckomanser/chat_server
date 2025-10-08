#pragma once

#include "boost/asio.hpp"

#include <deque>
#include <string>
#include <memory>
#include <chrono>

#include <chat/core/Command.h>
#include <chat/core/Message.h>
#include <chat/core/WriteQueue.h>
#include <chat/core/RateLimiter.h>
#include <chat/core/Types.h>

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
    void close();

    std::string get_remote_ip() const;
    const Nick& get_nick() const { return nick_; }
    std::weak_ptr<Server> get_server() const { return server_; }
    std::weak_ptr<Room> get_room() const { return room_; }
    std::string get_room_name() const;

    void set_nick(const Nick& nick) { nick_ = nick; }
    void set_room(const std::shared_ptr<Room>& room);

    bool allow_sending_now();

    bool is_authenticated() const { return authenticated_; }
    void set_authenticated(bool authenticated) { authenticated_ = authenticated; }
    const std::string get_username() const { return username_; }
    void set_username(const std::string username) { username_ = std::move(username); }
    const std::string get_nonce() const { return nonce_; }
    void set_nonce(const std::string nonce) { nonce_ = std::move(nonce); }

private:
    void do_read_line();
    void on_read(const boost::system::error_code& ec);
    void handle_command(const std::string& line);

    void send_info(const std::string& text) { deliver(format_info(text)); }
    void send_error(const std::string& text) { deliver(format_error(text)); }

private:
    tcp::socket socket_;
    asio::streambuf read_buf_;
    std::weak_ptr<Server> server_;
    WriteQueue writer_;
    Nick nick_;
    std::weak_ptr<Room> room_;
    RateLimiter rate_;
    std::unique_ptr<CommandRegistry> registry_;

    bool authenticated_ = false;
    std::string username_;
    std::string nonce_;
};
