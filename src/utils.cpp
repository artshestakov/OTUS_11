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
    { }

    return std::nullopt;
}
//-----------------------------------------------------------------------------
