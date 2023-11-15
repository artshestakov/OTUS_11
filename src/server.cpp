#include "server.h"
//-----------------------------------------------------------------------------
Server::Server(boost::asio::io_service& ios, short port)
    : m_IOS(ios), m_Acceptor(ios, tcp::endpoint(tcp::v4(), port))
{
    start_async_accept(std::make_shared<Session>(m_IOS));
}
//-----------------------------------------------------------------------------
Server::~Server()
{

}
//-----------------------------------------------------------------------------
void Server::handle_accept(std::shared_ptr<Session> s, const boost::system::error_code& e)
{
    if (e)
    {
        std::cout << "Can't accept connect from client: " + e.message() << std::endl;
        s.reset();
        return;
    }

    auto rmt = s->get_socket().remote_endpoint();
    std::cout << "Accept connection from " << rmt.address().to_string() << ":" << rmt.port() << std::endl;

    s->start_async_read();
    s = std::make_shared<Session>(m_IOS);
    start_async_accept(s);
}
//-----------------------------------------------------------------------------
void Server::start_async_accept(std::shared_ptr<Session> s)
{
    m_Acceptor.async_accept(s->get_socket(),
        boost::bind(&Server::handle_accept, this, s,
            boost::asio::placeholders::error));
}
//-----------------------------------------------------------------------------
