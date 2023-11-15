#include "session.h"
#include "utils.h"
//-----------------------------------------------------------------------------
Session::Session(boost::asio::io_service& ios)
    : m_Socket(ios)
{
    m_Data.resize(BUFFER_SIZE);
}
//-----------------------------------------------------------------------------
Session::~Session()
{

}
//-----------------------------------------------------------------------------
tcp::socket& Session::get_socket()
{
    return m_Socket;
}
//-----------------------------------------------------------------------------
void Session::start_async_read()
{
    m_Socket.async_read_some(
        boost::asio::buffer(m_Data, BUFFER_SIZE),
        boost::bind(&Session::handle_read, this, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}
//-----------------------------------------------------------------------------
void Session::handle_read(std::shared_ptr<Session>& s, const boost::system::error_code& e, size_t bytes)
{
    std::string client_address = utils::get_socket_address(s);

    if (e)
    {
        //Определим, а вдруг клиент отключился
        if (e == boost::asio::error::connection_reset ||
            e == boost::asio::error::eof)
        {
            std::cout << "Disconnected " << client_address << std::endl;
        }
        else //А вот тут уже ошибка
        {
            std::cout << "Can't read data from client " << client_address << ": " << e.message() << std::endl;
        }
        return;
    }

    std::string error_string;

    //Формируем пришедшие команды от клиента в список
    auto commands = utils::split_string(std::string(m_Data.begin(), m_Data.begin() + bytes), '\n');
    for (const auto& command : commands)
    {
        //Выполняем очередную команду, формируем ответ в зависимости от результата и отвечаем клиенту
        bool res = execute_command(command, error_string);
        
        std::string answer = res ?
            "OK" :
            "Error in command: " + command + "\n" + error_string;

        s->get_socket().write_some(boost::asio::buffer(answer, answer.size()));
    }

    //Чистим буфер и запускаем процесс асинхронного чтения
    std::fill(m_Data.begin(), m_Data.end(), '\0');
    start_async_read();
}
//-----------------------------------------------------------------------------
bool Session::execute_command(const std::string& cmd, std::string& error_string)
{
    //Парсим команду
    auto v = utils::split_string(cmd, ' ');
    if (v.size() == 0)
    {
        error_string = "Invalid format!";
        return false;
    }

    std::string command_type = v.front();
    utils::string_to_lower(command_type);

    if (command_type == "insert" && v.size() == 4)
    {
        return execute_insert(v, error_string);
    }
    else
    {
        error_string = "Invalid format!";
    }

    return false;
}
//-----------------------------------------------------------------------------
bool Session::execute_insert(const std::vector<std::string>& insert_vec, std::string& error_string)
{
    std::string table_name = insert_vec[1];

    uint64_t id = 0;
    if (auto a = utils::string_to_uint64(insert_vec[2]); a)
    {
        id = a.value();
    }
    else
    {
        error_string = "Invalid ID!";
        return false;
    }

    std::string name = insert_vec[3];

    m_Database[table_name].emplace_back(Table
        {
            id, name
        });

    return true;
}
//-----------------------------------------------------------------------------
