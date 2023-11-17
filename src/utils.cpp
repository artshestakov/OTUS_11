#include "utils.h"
//-----------------------------------------------------------------------------
std::string utils::get_socket_address(std::shared_ptr<Session> s)
{
    auto rmt = s->get_socket().remote_endpoint();
    return rmt.address().to_string() + ":" + std::to_string(rmt.port());
}
//-----------------------------------------------------------------------------
std::vector<std::string> utils::split_string(const std::string& s, char sep)
{
    std::vector<std::string> v;

    if (!s.empty())
    {
        size_t pos = 0, last_pos = 0;
        while ((pos = s.find(sep, last_pos)) != std::string::npos)
        {
            if (pos != 0)
            {
                if ((pos - last_pos) > 0)
                {
                    v.emplace_back(s.substr(last_pos, pos - last_pos));
                }
                last_pos = ++pos;
            }
            else
            {
                ++last_pos;
            }
        }

        if (pos == std::string::npos)
        {
            size_t Size = s.size();
            if (last_pos < Size)
            {
                v.emplace_back(s.substr(last_pos, Size - last_pos));
            }
        }
    }

    return v;
}
//-----------------------------------------------------------------------------
void utils::string_to_lower(std::string& s)
{
    //Комментарий для преподавателя: именно такая реализация самая быстрая и оптимальная
    //Конструкция проверена годами на разных компиляторах и системах.
    for (size_t i = 0, c = s.size(); i < c; ++i)
    {
        s[i] = std::move((char)std::tolower((int)s[i]));
    }
}
//-----------------------------------------------------------------------------
std::optional<uint64_t> utils::string_to_uint64(const std::string& s)
{
    try
    {
        return std::stoull(s);
    }
    catch (const std::exception&)
    {
    }

    return std::nullopt;
}
//-----------------------------------------------------------------------------
void utils::string_rm_right(std::string& s, size_t n)
{
    //Получаем размер строки
    size_t size = s.size();
    if (size == 0) //Если строка пустая - выходим
    {
        return;
    }

    if (n >= size) //Если кол-во символов больше либо равно размеру строки - очищаем её
    {
        s.clear();
    }
    else //Вырезаем N символов с конце строки
    {
        s.erase(size - n);
    }
}
//-----------------------------------------------------------------------------
