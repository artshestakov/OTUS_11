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

    std::string data;

    //Если данные пришли - формируем вывод на консоль
    if (bytes > 0)
    {
        data = std::string(m_Data.begin(), m_Data.begin() + bytes);

        //Учтём, что последним символом может быть перевод строки. Он нам не нужен
        if (data.back() == '\n')
        {
            data.pop_back();
            --bytes;
        }
    }
    else //Данных нет - так и сообщим
    {
        data = "NO DATA";
    }

    //Выводим на консоль только в случае, если что-то осталось после формирования выше
    if (bytes > 0)
    {
        std::cout << "Accept data from " << client_address << ": "  << data << std::endl;
    }

    //Чистим буфер и запускаем процесс асинхронного чтения
    std::fill(m_Data.begin(), m_Data.end(), '\0');
    start_async_read();
}
//-----------------------------------------------------------------------------
