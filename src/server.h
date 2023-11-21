#pragma once
//-----------------------------------------------------------------------------
#include "session.h"
//-----------------------------------------------------------------------------
class Server
{
public:
    Server(boost::asio::io_service& ios, int port);
    ~Server();

    void handle_accept(std::shared_ptr<Session> s, const boost::system::error_code& e);

private:
    void start_async_accept(std::shared_ptr<Session> s);

private:
    boost::asio::io_service& m_IOS;
    tcp::acceptor m_Acceptor;
};
//-----------------------------------------------------------------------------
