#include <chat/transport/Session.h>

#include <chat/transport/Server.h>
#include <chat/protocol/Router.h>
#include <chat/protocol/Aliases.h>
#include <chat/protocol/JsonCodec.h>
#include <chat/core/Log.h>

Session::Session(tcp::socket&& socket, std::shared_ptr<Server> server)  //
    : stream_(std::move(socket), server->get_tls_ctx()),                //
      server_(std::move(server)),                                       //
      writer_(stream_)
{
}

void Session::start()
{
    auto self = shared_from_this();
    writer_.on_error_ = [self](auto ec)
    {
        log_line("ERROR", "session", "write: " + ec.message());
        self->close();
    };

    stream_.async_handshake(ssl::stream_base::server,  //
        [self, this](auto ec)
        {
            if (ec)
            {
                log_line("ERROR", "session", "tls : " + ec.message());
                return;
            }

            send(JsonCodec::to_line(Envelope::hello({"tls", "digest", "json", "aliases"})));
            send(JsonCodec::to_line(Envelope::info("use /auth <user> then /auth_resp <user> <response_hex>")));

            self->do_read_line();
        });
}

void Session::close()
{
    boost::system::error_code ec;
    if (auto server = server_.lock()) server->remove(shared_from_this());
    stream_.shutdown();
    stream_.lowest_layer().shutdown(tcp::socket::shutdown_both, ec);
    stream_.lowest_layer().close(ec);
}

void Session::do_read_line()
{
    auto self = shared_from_this();
    asio::async_read_until(stream_, buf_, '\n',  //
        [self](auto ec, size_t n) { self->on_read(ec, n); });
}

void Session::on_read(boost::system::error_code& ec, size_t)
{
    if (ec)
    {
        close();
        return;
    }

    std::istream is(&buf_);
    std::string line;
    std::getline(is, line);
    if (!line.empty() && line.back() == '\r') line.pop_back();

    if (auto line2 = alias_to_json_line(line)) line = *line2;

    if (auto server = server_.lock())
    {
        auto router = server->get_router();
        std::string reply = router->handle(line, authenticated_, username_);
        send(reply);

        if (reply.find("\"bye\"") != std::string::npos)
        {
            close();
            return;
        }
    }

    do_read_line();
}

void Session::send(const std::string& line)
{
    writer_.enqueue(line);
}
