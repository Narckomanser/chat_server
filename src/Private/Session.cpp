#include "../Public/Session.h"

#include <iostream>

#include "../Public/Server.h"

constexpr std::size_t kMaxLine = 1024;

Session::Session(tcp::socket&& socket, std::shared_ptr<Server> server) : socket_(std::move(socket)), server_(std::move(server)) {}

void Session::start()
{
    std::cout << "[session] INFO: get connection from " << get_remote_ip() << std::endl;

    deliver("hello from asio\n");
    do_read_line();
}

void Session::deliver(std::string line)
{
    if (line.size() > kMaxLine)
    {
        line.resize(kMaxLine);
        line.push_back('\n');
    }

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
            std::cout << "[INFO] session closed ip=" << get_remote_ip() << " reason=" << ec.message() << std::endl;
        }
        close();
        return;
    }

    std::istream is(&read_buf_);
    std::string line;
    std::getline(is, line);

    line.push_back('\n');
    deliver(std::move(line));

    do_read_line();
}

void Session::do_write()
{
    auto self = shared_from_this();
    asio::async_write(
        socket_,
        asio::buffer(outbox_.front().data(),
        outbox_.front().size()),
        [self](const boost::system::error_code& ec, std::size_t) { self->on_write(ec); });
}

void Session::on_write(const boost::system::error_code& ec)
{
    if (ec)
    {
        if (ec != asio::error::operation_aborted)
        {
            std::cout << "[ERROR] write failed ip=" << get_remote_ip() << " ec=" << ec.message() << std::endl;
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
    socket_.shutdown(tcp::socket::shutdown_both, ignore);
    socket_.close(ignore);

    if (auto srv = server_.lock())
    {
        srv->remove_session(shared_from_this());
    }
}
