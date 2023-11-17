#pragma once
//-----------------------------------------------------------------------------
#include "stdafx.h"
//-----------------------------------------------------------------------------
class Session : public std::enable_shared_from_this<Session>
{
private:
    struct Record
    {
        uint64_t ID;
        std::string Name;
    };
    using Table = std::vector<Record>;

    struct SessionContext
    {
        std::string ErrorMessage;
        std::string Answer;
    };

public:
    Session(boost::asio::io_service& ios);
    ~Session();

    tcp::socket& get_socket();
    void start_async_read();
    void handle_read(std::shared_ptr<Session>& s, const boost::system::error_code& e, size_t bytes);

private:
    bool execute_command(SessionContext& ctx, const std::string& cmd);

    bool execute_insert(SessionContext& ctx, const std::vector<std::string>& insert_vec);
    bool execute_truncate(SessionContext& ctx, const std::string& table_name);
    bool execute_intersection(SessionContext& ctx);
    bool execute_symmetric_difference(SessionContext& ctx);

    //Дополнительные методы вне условия задачи для демонистрации
    bool execute_select(SessionContext& ctx, const std::string& table_name);
    bool execute_selectall(SessionContext& ctx);
    bool execute_delete(SessionContext& ctx, const std::string& table_name, const std::string& id_str);

private:
    std::optional<uint64_t> string_to_uint64(SessionContext& ctx, const std::string& s);
    Session::Table* get_table(const std::string& table_name, SessionContext* ctx = nullptr);
    bool exists_id(const Table& table, uint64_t id);
    std::string get_name(const std::string& table_name, uint64_t id);

private:
    tcp::socket m_Socket;
    std::vector<char> m_Data;
    std::unordered_map<std::string, Table> m_Database;
};
//-----------------------------------------------------------------------------
