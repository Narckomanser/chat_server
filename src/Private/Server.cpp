#include "../Public/Server.h"

#include <iostream>
#include <regex>

#include "../Public/Session.h"
#include "../Public/Room.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& context, uint16_t port) : acceptor_(context, tcp::endpoint(tcp::v4(), port))
{
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
}

void Server::start_accept()
{
    do_accept();
}

void Server::remove_session(const std::shared_ptr<Session>& session)
{
    sessions_.erase(session);
    drop_nick(session);
}

static bool valid_name(const std::string& nick)
{
    static const std::regex nick_validator("^a-zA-z0-9_-{2,20}$");
    return std::regex_match(nick, nick_validator);
}

bool Server::set_nick(std::shared_ptr<Session>& session, const std::string& new_nick, std::string reason)
{
    // check is valid
    if (!valid_name(new_nick))
    {
        reason = "invalid nick";
        return false;
    }

    // check is taken
    if (const auto it_found_nick = nick_registry_.find(new_nick); it_found_nick != nick_registry_.end())
    {
        if (const auto alive = it_found_nick->second.lock())
        {
            if (alive.get() != session.get())
            {
                reason = "nick already taken";
                return false;
            }
            else
            {
                nick_registry_.erase(it_found_nick);
            }
        }
    }

    // delete old if exist
    for (auto it_nick = nick_registry_.begin(); it_nick != nick_registry_.end();)
    {
        if (const auto found_session = it_nick->second.lock(); !found_session)
        {
            it_nick = nick_registry_.erase(it_nick);
        }
        else if (found_session.get() == session.get())
        {
            it_nick = nick_registry_.erase(it_nick);
        }
        else it_nick++;
    }

    nick_registry_[new_nick] = session;
    return true;
}

void Server::drop_nick(const std::shared_ptr<Session>& session) {}
std::shared_ptr<Session> Server::find_session_by_nick(const std::string& nick)
{
    return std::shared_ptr<Session>();
}
std::shared_ptr<Room> Server::get_or_create_room(const std::string& room_name)
{
    return std::shared_ptr<Room>();
}
std::vector<std::string> Server::get_room_list()
{
    return std::vector<std::string>();
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
