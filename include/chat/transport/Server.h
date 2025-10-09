#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <unordered_set>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace ssl = asio::ssl;

class Router;
class Session;

class Server : public std::enable_shared_from_this<Server>
{
public:
    Server(                     //
        asio::io_context& io,   //
        unsigned short port,    //
        std::string cert_file,  //
        std::string key_file,   //
        std::shared_ptr<Router> router);

    void start();

    ssl::context& get_tls_ctx() { return tls_ctx_; }
    std::shared_ptr<Router> get_router() { return router_; }
    void remove(const std::shared_ptr<Session>& session);

private:
    void do_accept();
    void on_accept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& ec);
    void load_certs(const std::string& cert, const std::string& key);

private:
    tcp::acceptor acceptor_;
    ssl::context tls_ctx_;
    std::shared_ptr<Router> router_;
    std::unordered_set<std::shared_ptr<Session>> sessions_;
};