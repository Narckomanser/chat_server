#pragma once

#include "boost/asio.hpp"

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include <chat/server/NickRegistry.h>

class Session;
class Room;

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Server : public std::enable_shared_from_this<Server>
{
public:
    Server(asio::io_context& context, uint16_t port);
    ~Server();

    void start_accept();
    void remove_session(const std::shared_ptr<Session>& session);

    bool set_nick(const std::shared_ptr<Session>& session, const Nick& new_nick, std::string& reason);
    void drop_nick(const Nick& nick);
    std::shared_ptr<Session> find_session_by_nick(const Nick& nick);

    std::shared_ptr<Room> get_or_create_room(const std::string& room_name);
    std::vector<std::string> get_room_list();
    std::vector<std::pair<std::string, std::size_t>> get_list_rooms_detailed() const;
    std::shared_ptr<Room> find_room(const std::string& name) const;
    void prune_empty_room(const std::string& name);

    bool is_user_exists(const std::string& username) const;
    bool insert_user(const std::string& username, const std::string& ha1_hex);
    std::optional<std::string> get_ha1(const std::string& username) const;
    const std::string realm() { return realm_; }

private:
    void do_accept();
    void on_accept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& ec);

    void init_db();

private:
    void* db_ = nullptr;

    std::string realm_ = "accused_chat";
    tcp::acceptor acceptor_;
    std::unordered_set<std::shared_ptr<Session>> sessions_;
    NickRegistry nick_registry_;
    std::unordered_map<std::string, std::shared_ptr<Room>> rooms_;
};