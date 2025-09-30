#include "../Public/Server.h"

#include <iostream>
#include <regex>

#include "../Public/Session.h"
#include "../Public/Room.h"
#include "../Public/Log.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& context, uint16_t port) : acceptor_(context)
{
    boost::system::error_code ec;

    tcp::endpoint endpoint(tcp::v4(), port);

    acceptor_.open(endpoint.protocol(), ec);
    if (ec) throw std::runtime_error("acceptor.open: " + ec.message());

    acceptor_.set_option(asio::socket_base::reuse_address(true), ec);
    if (ec) throw std::runtime_error("acceptor.set_option: " + ec.message());

    acceptor_.bind(endpoint, ec);
    if (ec) throw std::runtime_error("acceptor.bind: " + ec.message());

    acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) throw std::runtime_error("acceptor.listen: " + ec.message());

    log_line("INFO", "server", "listening");
}

void Server::start_accept()
{
    do_accept();
}

void Server::remove_session(const std::shared_ptr<Session>& session)
{
    sessions_.erase(session);
    drop_nick(session->get_nick());
}

bool Server::set_nick(const std::shared_ptr<Session>& session, const Nick& new_nick, std::string& reason)
{
    return nick_registry_.set(session, new_nick, reason);
}

void Server::drop_nick(const Nick& nick)
{
    nick_registry_.drop(nick);
}

std::shared_ptr<Session> Server::find_session_by_nick(const Nick& nick)
{
    return nick_registry_.find(nick.str());
}

std::shared_ptr<Room> Server::get_or_create_room(const std::string& room_name)
{
    if (const auto it_room = rooms_.find(room_name); it_room != rooms_.end())
    {
        return it_room->second;
    }

    const auto new_room = std::make_shared<Room>(room_name, shared_from_this());
    rooms_[room_name] = new_room;
    return new_room;
}

std::vector<std::string> Server::get_room_list()
{
    if (rooms_.empty()) return {};

    std::vector<std::string> room_list;
    room_list.reserve(rooms_.size());
    for (const auto& room : rooms_)
    {
        room_list.push_back(room.first);
    }

    return room_list;
}

std::vector<std::pair<std::string, std::size_t>> Server::get_list_rooms_detailed() const
{
    std::vector<std::pair<std::string, std::size_t>> rooms;
    rooms.reserve(rooms_.size());
    for (auto const& [name, room] : rooms_)
    {
        rooms.emplace_back(name, room ? room->size() : 0);
    }
    return rooms;
}

std::shared_ptr<Room> Server::find_room(const std::string& name) const
{
    if (auto it = rooms_.find(name); it != rooms_.end()) return it->second;
    return {};
}

void Server::prune_empty_room(const std::string& name)
{
    if (auto it_room = rooms_.find(name); it_room != rooms_.end())
    {
        if (it_room->second->get_members().empty())
        {
            rooms_.erase(it_room);
        }
    }
}

void Server::do_accept()
{
    auto socket = std::make_shared<tcp::socket>(acceptor_.get_executor());
    acceptor_.async_accept(*socket, std::bind(&Server::on_accept, this, socket, std::placeholders::_1));
}

void Server::on_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& ec)
{
    do_accept();

    if (ec)
    {
        log_line("ERROR", "accept", ec.message());
        return;
    }

    auto session = std::make_shared<Session>(std::move(*socket), shared_from_this());
    sessions_.insert(session);

    log_line("INFO", "accept", "new session");

    session->start();
}
