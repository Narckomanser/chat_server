#include <chat/transport/Server.h>

#include <chat/transport/Session.h>
#include <chat/core/Log.h>

Server::Server(asio::io_context& io,  //
    unsigned short port,              //
    std::string cert_file,            //
    std::string key_file,             //
    std::shared_ptr<Router> router)
    : acceptor_(io, tcp::endpoint(tcp::v4(), port)),  //
      tls_ctx_(ssl::context::tls_server),             //
      router_(std::move(router))
{
    tls_ctx_.set_options(                                    //
        ssl::context::default_workarounds |                  //
        ssl::context::no_sslv2 | ssl::context::no_sslv3 |    //
        ssl::context::no_tlsv1 | ssl::context::no_tlsv1_1 |  //
        ssl::context::single_dh_use);

    load_certs(cert_file, key_file);
}

void Server::start()
{
    do_accept();
}

void Server::remove(const std::shared_ptr<Session>& session)
{
    sessions_.erase(session);
}

void Server::do_accept()
{
    auto socket = std::make_shared<tcp::socket>(acceptor_.get_executor());
    acceptor_.async_accept(                  //
        *socket,                             //
        [self = shared_from_this(), socket]  //
        (auto ec)                            //
        { self->on_accept(socket, ec); });
}

void Server::on_accept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& ec)
{
    do_accept();

    if (ec)
    {
        log_line("ERROR", "server", ec.message());
        return;
    }

    auto session = std::make_shared<Session>(std::move(*socket), shared_from_this());
    sessions_.insert(session);
    session->start();
}

void Server::load_certs(const std::string& cert, const std::string& key)
{
    boost::system::error_code ec;

    tls_ctx_.use_certificate_chain_file(cert, ec);
    if (ec) throw std::runtime_error("use_certificate_chain_file: " + ec.message());

    tls_ctx_.use_private_key_file(key, ssl::context::pem, ec);
    if (ec) throw std::runtime_error("use_private_key_file: " + ec.message());
}
