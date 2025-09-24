#include "../Public/Session.h"

Session::Session(tcp::socket&& socket, std::shared_ptr<Server> server)
    : socket_(std::move(socket)),
      server_(std::move(server))
{

}

void Session::start()
{
    std::cout << "[session] INFO: get connection from " << socket_.remote_endpoint().address().to_string() << std::endl;

    /* залогировать подключение (`remote_ip()`)
* (для шага-минимум) отправить привет: `deliver("hello from asio\n")`
    * запустить `do_read_line()`*/
}
void Session::deliver(std::string line)
{
    /*
     *  положить в `outbox_`
* если **не идёт** активная запись — `do_write()`*/
}
std::string Session::get_remote_ip() const
{

}
void Session::do_read_line()
{
    /*  `async_read_until(socket_, read_buf_, '\n', on_read)`*/
}
void Session::on_read(const boost::system::error_code& ec, std::size_t size)
{
    /*
     * если `ec` → `close()` и `return`
* распарсить строку из буфера (вынуть из `read_buf_`)
* (пока MVP) сделать эхо: `deliver(line)`
* снова `do_read_line()` (петля)*/
}
void Session::do_write()
{
    /*взять `outbox_.front()`, `async_write(socket_, buffer(front), on_write)`*/
}
void Session::on_write(const boost::system::error_code& ec, std::size_t size)
{
    /*
     * * если `ec` → `close()` и `return`
* `outbox_.pop_front()`
* если `outbox_` не пуст — снова `do_write()`*/
}
void Session::close()
{
    /*
     * * аккуратно закрыть `socket_`
* попросить `server_->remove_session(shared_from_this())`*/
}