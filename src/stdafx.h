#pragma once
//-----------------------------------------------------------------------------
#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
//-----------------------------------------------------------------------------
using boost::asio::ip::tcp;
//-----------------------------------------------------------------------------
inline constexpr size_t BUFFER_SIZE = /*1024*/4;
//-----------------------------------------------------------------------------
