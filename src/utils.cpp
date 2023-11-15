#include "utils.h"
//-----------------------------------------------------------------------------
std::string utils::get_socket_address(std::shared_ptr<Session> s)
{
    auto rmt = s->get_socket().remote_endpoint();
    return rmt.address().to_string() + ":" + std::to_string(rmt.port());
}
//-----------------------------------------------------------------------------
