#pragma once
//-----------------------------------------------------------------------------
#include "session.h"
//-----------------------------------------------------------------------------
namespace utils
{
    std::string get_socket_address(std::shared_ptr<Session> s);
    std::vector<std::string> split_string(const std::string& s, char sep);
    void string_to_lower(std::string& s);
    std::optional<uint64_t> string_to_uint64(const std::string& s);
    void string_rm_right(std::string& s, size_t n);
}
//-----------------------------------------------------------------------------
