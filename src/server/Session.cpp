#include <chat/server/Session.h>

#include <iostream>
#include <sstream>
#include <atomic>
#include <utility>

#include <chat/server/Server.h>
#include <chat/server/Room.h>
#include <chat/core/Command.h>
#include <chat/core/Log.h>

using namespace std::literals;

constexpr std::size_t kMaxLine = 1024;

Session::Session(tcp::socket&& socket, const std::shared_ptr<Server>& server)
    : socket_(std::move(socket)), server_(server), writer_(socket_)
{
}

void Session::start()
{
    auto self = shared_from_this();
    writer_.on_error_ = [self, this](const boost::system::error_code& ec)
    {
        log_line("ERROR", "write", "failed ip= " + get_remote_ip() + " ec= " + ec.message());
        close();
    };

    log_line("INFO", "session", std::string("connected ip=") + get_remote_ip());

    static std::atomic_uint64_t counter{1};
    if (auto server = server_.lock())
    {
        for (;;)
        {
            // TODO should be replaced someday
            Nick candidate;
            candidate.name_ = "anon" + std::to_string(counter++ % 100000);
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

    send_info("welcome! your nick is " + nick_.name_);

    registry_ = build_default_registry(server_);

    do_read_line();
}

void Session::deliver(std::string line)
{
    writer_.enqueue(std::move(line));
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

std::string Session::get_remote_ip() const
{
    boost::system::error_code ec;
    auto ep = socket_.remote_endpoint(ec);

    if (ec) return {};

    return ep.address().to_string();
}

std::string Session::get_room_name() const
{
    if (const auto room = room_.lock()) return room->get_room_name();
    return {};
}

void Session::set_room(const std::shared_ptr<Room>& room)
{
    room_ = room;
}

bool Session::allow_sending_now()
{
    return rate_.allow();
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
            if (!rate_.allow())
            {
                log_line("WARN", "rate", std::string("rate-limit nick=") + nick_.name_);
                send_error("rate limit");
                do_read_line();
                return;
            }
            room->broadcast(format_public(get_room_name(), nick_.name_, line));
        }
        else
            send_error("not in room; use /join <room>");
    }

    do_read_line();
}

void Session::handle_command(const std::string& line)
{
    auto cmd = parse_command(line);
    if (cmd.kind == commandKind::Unknown || !registry_ || !registry_->dispatch(*this, cmd))
    {
        send_error("unknown command");
    }
}
