#include "../Public/Server.h"

#include <iostream>
#include <regex>

#include "../Public/Session.h"
#include "../Public/Room.h"

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

static bool valid_name(const std::string& nick)
{
    static const std::regex nick_validator("^[A-Za-z0-9_-]{2,20}$");
    return std::regex_match(nick, nick_validator);
}

bool Server::set_nick(const std::shared_ptr<Session>& session, const std::string& new_nick, std::string& reason)
{
    // check is valid by regex
    if (!valid_name(new_nick))
    {
        reason = "invalid nick";
        return false;
    }

    // check is taken
    if (const auto it_nick = nick_registry_.find(new_nick); it_nick != nick_registry_.end())
    {
        if (const auto alive = it_nick->second.lock())
        {
            if (alive.get() != session.get())
            {
                reason = "nick already taken";
                return false;
            }
            else
            {
                drop_nick(alive->get_nick());
            }
        }
    }

    // delete old if exist
    drop_nick(session->get_nick());

    nick_registry_[new_nick] = session;
    return true;
}

void Server::drop_nick(const std::string& nick)
{
        nick_registry_.erase(nick);
}

std::shared_ptr<Session> Server::find_session_by_nick(const std::string& nick)
{
    if (auto it_nick = nick_registry_.find(nick); it_nick != nick_registry_.end())
    {
        if (auto it_session = it_nick->second.lock())
        {
            return it_session;
        }
        // lazy remove if weak_ptr is dead
        nick_registry_.erase(it_nick);
    }
    return {};
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
        std::cout << "[server] ERROR: " << ec.message() << std::endl;
        return;
    }

    auto session = std::make_shared<Session>(std::move(*socket), shared_from_this());
    sessions_.insert(session);
    session->start();
}
