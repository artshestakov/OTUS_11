#pragma once
//-----------------------------------------------------------------------------
#include "stdafx.h"
//-----------------------------------------------------------------------------
class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(boost::asio::io_service& ios);
    ~Session();

    tcp::socket& get_socket();
    void start_async_read();
    void handle_read(std::shared_ptr<Session>& s, const boost::system::error_code& e, size_t bytes);

private:
    tcp::socket m_Socket;
    std::vector<char> m_Data;
};
//-----------------------------------------------------------------------------
