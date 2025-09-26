#pragma once

#include "boost/asio.hpp"

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class Session;
class Room;

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Server : public std::enable_shared_from_this<Server>
{
public:
    Server(asio::io_context& context, uint16_t port);

    void start_accept();
    void remove_session(const std::shared_ptr<Session>& session);

    bool set_nick(const std::shared_ptr<Session>& session, const std::string& new_nick, std::string& reason);
    void drop_nick(const std::shared_ptr<Session>& session);
    std::shared_ptr<Session> find_session_by_nick(const std::string& nick);

    std::shared_ptr<Room> get_or_create_room(const std::string& room_name);
    std::vector<std::string> get_room_list();

private:
    void do_accept();
    void on_accept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& ec);


private:
    tcp::acceptor acceptor_;
    std::unordered_set<std::shared_ptr<Session>> sessions_;
    std::unordered_map<std::string, std::weak_ptr<Session>>nick_registry_;
    std::unordered_map<std::string, std::shared_ptr<Room>> rooms_;
};