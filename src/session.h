#pragma once
//-----------------------------------------------------------------------------
#include "stdafx.h"
//-----------------------------------------------------------------------------
class Session : public std::enable_shared_from_this<Session>
{
private:
    struct Table
    {
        uint64_t ID;
        std::string Name;
    };

public:
    Session(boost::asio::io_service& ios);
    ~Session();

    tcp::socket& get_socket();
    void start_async_read();
    void handle_read(std::shared_ptr<Session>& s, const boost::system::error_code& e, size_t bytes);

private:
    bool execute_command(const std::string& cmd, std::string &error_string);

private:
    tcp::socket m_Socket;
    std::vector<char> m_Data;
    std::unordered_map<std::string, std::vector<Table>> m_Database;
};
//-----------------------------------------------------------------------------
