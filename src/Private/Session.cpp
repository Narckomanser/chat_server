#include "../Public/Session.h"

#include <iostream>
#include <sstream>
#include <regex>
#include <atomic>

#include "../Public/Server.h"
#include "../Public/Room.h"
#include "../Public/Log.h"

using namespace std::literals;

constexpr std::size_t kMaxLine = 1024;

Session::Session(tcp::socket&& socket, std::shared_ptr<Server> server) : socket_(std::move(socket)), server_(std::move(server)) {}

void Session::start()
{
    log_line("INFO", "session", std::string("connected ip=") + get_remote_ip());

    static std::atomic_uint64_t counter{1};
    if (auto server = server_.lock())
    {
        for (;;)
        {
            // TODO should be replaced someday
            std::string candidate = "anon" + std::to_string(counter++ % 100000);
            std::string reason;
            if (server->set_nick(shared_from_this(), candidate, reason))
            {
                nick_ = std::move(candidate);
                break;
            }
            std::cout << reason << std::endl;
        }
    }

    send_info("__________________________________________________________");
    send_info("/nick <new nick>                     to change nickname");
    send_info("/join <room name>                    to join the room or create the new one");
    send_info("/leave                               to leave the room");
    send_info("/rooms                               to get list of rooms");
    send_info("/msg <interlocutor name> <text>      to send message");
    send_info("/quit                                to end session");
    send_info("__________________________________________________________");

    send_info("welcome! your nick is " + nick_);

    do_read_line();
}

void Session::deliver(std::string line)
{
    outbox_.push_back(std::move(line));

    if (outbox_.size() == 1)
    {
        do_write();
    }
}

std::string Session::get_remote_ip() const
{
    boost::system::error_code ec;
    auto ep = socket_.remote_endpoint(ec);

    if (ec) return {};

    return ep.address().to_string();
}

void Session::set_room(const std::shared_ptr<Room>& room)
{
    room_ = room;
}

std::string Session::get_room_name() const
{
    if (const auto room = room_.lock()) return room->get_room_name();
    return {};
}

void Session::do_read_line()
{
    auto self = shared_from_this();
    asio::async_read_until(socket_, read_buf_, '\n', [self](const boost::system::error_code& ec, std::size_t) { self->on_read(ec); });
}

void Session::on_read(const boost::system::error_code& ec)
{
    if (ec)
    {
        if (ec != asio::error::operation_aborted)
        {
            log_line("INFO", "session", std::string("closed ip=") + get_remote_ip() + " ec=" + ec.message());
        }
        close();
        return;
    }

    std::istream is(&read_buf_);
    std::string line;
    std::getline(is, line);

    if (!line.empty() && line.back() == '\r') line.pop_back();

    if (line.size() > kMaxLine)
    {
        send_error("line too long");
        do_read_line();
        return;
    }

    if (!line.empty() && line[0] == '/')
    {
        handle_command(line);
    }
    else
    {
        if (auto room = room_.lock())
        {
            if (!allow_message())
            {
                log_line("WARN", "rate", std::string("rate-limit nick=") + nick_);
                do_read_line();
                return;
            }
            std::string message = room->get_room_name() + "/" + nick_ + ": " + line + "\n";
            room->broadcast(message);
        }
        else
            send_error("not in room; use /join <room>");
    }

    do_read_line();
}

void Session::do_write()
{
    auto self = shared_from_this();
    asio::async_write(socket_, asio::buffer(outbox_.front().data(), outbox_.front().size()),
        [self](const boost::system::error_code& ec, std::size_t) { self->on_write(ec); });
}

void Session::on_write(const boost::system::error_code& ec)
{
    if (ec)
    {
        if (ec != asio::error::operation_aborted)
        {
            log_line("ERROR", "session", std::string("write failed ip=") + get_remote_ip() + " ec=" + ec.message());
        }
        close();
        return;
    }

    if (!outbox_.empty())
    {
        outbox_.pop_front();
        do_write();
    }
}

void Session::close()
{
    boost::system::error_code ignore;

    if (auto room = room_.lock())
    {
        room->leave(shared_from_this());
    }

    socket_.shutdown(tcp::socket::shutdown_both, ignore);
    socket_.close(ignore);

    if (auto server = server_.lock())
    {
        server->remove_session(shared_from_this());
    }
}

static bool valid_name(const std::string& nick)
{
    static const std::regex nick_validator("^[A-Za-z0-9_-]{2,20}$");
    return std::regex_match(nick, nick_validator);
}

void Session::handle_command(const std::string& line)
{
    // skip the "/" symbol
    std::istringstream iss(line.substr(1));
    std::string cmd;
    iss >> cmd;

    if (cmd == "nick")
    {
        std::string new_nick;
        iss >> new_nick;
        if (new_nick.empty() || !valid_name(new_nick))
        {
            send_error("usage: /nick <name>");
            return;
        }
        if (const auto server = server_.lock())
        {
            std::string reason;
            if (server->set_nick(shared_from_this(), new_nick, reason))
            {
                nick_ = new_nick;
                send_info("nick set to " + nick_);
            }
            else
                send_error(reason);
        }
    }
    else if (cmd == "join")
    {
        std::string room_to_join;
        iss >> room_to_join;
        if (room_to_join.empty() || !valid_name(room_to_join))
        {
            send_error("usage: /join <room>");
            return;
        }
        if (const auto server = server_.lock())
        {
            const auto new_room = server->get_or_create_room(room_to_join);
            if (const auto old_room = room_.lock()) old_room->leave(shared_from_this());
            set_room(new_room);
            new_room->join(shared_from_this());
            send_info("joined " + room_to_join);
        }
    }
    else if (cmd == "leave")
    {
        if (const auto room = room_.lock())
        {
            const auto room_name = room->get_room_name();
            room->leave(shared_from_this());
            room_.reset();
            send_info("left " + room_name);
        }
        else
            send_error("not in room");
    }
    else if (cmd == "rooms")
    {
        if (const auto server = server_.lock())
        {
            const auto room_list = server->get_list_rooms_detailed();
            if (room_list.empty())
                send_info("rooms <none>");
            else
            {
                for (auto const& [name, size] : room_list)
                {
                    send_info("room: " + name + " members= " + std::to_string(size));
                }
            }
        }
    }
    else if (cmd == "msg")
    {
        std::string to;
        iss >> to;

        std::string text;
        getline(iss, text);

        if (to.empty() || text.empty())
        {
            send_error("usage: /msg <nick> <text>");
            return;
        }

        if (!allow_message()) return;

        if (text[0] == ' ') text.erase(0, 1);

        if (const auto server = server_.lock())
        {
            if (const auto dst = server->find_session_by_nick(to))
            {
                dst->deliver("[PM]" + nick_ + ": " + text + "\n");
                deliver("[PM]to " + to + ": " + text + "\n");
            }
            else
                send_error("nick not found");
        }
    }
    else if (cmd == "quit")
    {
        close();
    }
    else
        send_error("unknown command");
}

bool Session::allow_message()
{
    const auto now = std::chrono::steady_clock::now();
    const auto cutoff = now - 1s;

    while (!msg_times_.empty() && msg_times_.front() < cutoff)
    {
        msg_times_.pop_front();
    }

    if (msg_times_.size() >= 10)
    {
        send_error("rate limit");
        return false;
    }

    msg_times_.push_back(now);
    return true;
}