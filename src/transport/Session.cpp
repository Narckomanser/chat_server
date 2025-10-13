#include <chat/transport/Session.h>
#include <chat/transport/Server.h>

#include <chat/protocol/Router.h>
#include <chat/protocol/Aliases.h>
#include <chat/protocol/JsonCodec.h>
#include <chat/protocol/dto/Dto.h>
#include <chat/protocol/SessionCtx.h>

#include <chat/core/Log.h>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>
#include <utility>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace ssl = boost::asio::ssl;

Session::Session(tcp::socket&& sock, std::shared_ptr<Server> srv)
    : stream_(std::move(sock), srv->get_tls_ctx()), server_(std::move(srv)), writer_(stream_)
{
}

void Session::start()
{
    auto self = shared_from_this();

    writer_.on_error_ = [self](const boost::system::error_code& ec)
    {
        log_line("ERROR", "write", ec.message());
        self->close();
    };

    stream_.async_handshake(ssl::stream_base::server,
        [self](const boost::system::error_code& ec)
        {
            if (ec)
            {
                log_line("ERROR", "tls", ec.message());
                self->close();
                return;
            }

            self->ctx_.authenticated = false;
            self->ctx_.username.clear();
            self->ctx_.current_room.clear();
            self->ctx_.push = [self](const Envelope& e) { self->send(JsonCodec::to_line(e)); };

            self->send(JsonCodec::to_line(Envelope::hello({"tls", "digest", "json", "aliases"})));
            self->send(JsonCodec::to_line(Envelope::info("use /auth <user> then /auth_resp <user> <response_hex>")));

            self->do_read_line();
        });
}

void Session::do_read_line()
{
    auto self = shared_from_this();
    asio::async_read_until(stream_, buf_, '\n', [self](const boost::system::error_code& ec, std::size_t n) { self->on_read(ec, n); });
}

static inline void strip_cr(std::string& s)
{
    if (!s.empty() && (s.back() == '\r' || s.back() == '\n')) s.pop_back();
}

void Session::on_read(const boost::system::error_code& ec, std::size_t)
{
    if (ec)
    {
        self_close_();
        return;
    }

    std::istream is(&buf_);
    std::string line;
    std::getline(is, line);
    strip_cr(line);

    if (auto jline = alias_to_json_line(line))
    {
        line = *jline;
    }

    if (auto srv = server_.lock())
    {
        auto router = srv->get_router();
        std::string reply = router->Handle(line, ctx_);
        send(reply);

        if (reply.find("\"bye\"") != std::string::npos)
        {
            close();
            return;
        }
    }
    else
    {
        close();
        return;
    }

    do_read_line();
}

void Session::send(const std::string& s)
{
    writer_.enqueue(s);
}

void Session::close()
{
    boost::system::error_code ign;

    stream_.shutdown(ign);
    stream_.lowest_layer().shutdown(tcp::socket::shutdown_both, ign);
    stream_.lowest_layer().close(ign);

    if (auto srv = server_.lock())
    {
        srv->remove(shared_from_this());
    }
}

void Session::self_close_()
{
    close();
}
