// -*- c++ -*-
#if !defined(NET_H)
#define NET_H 1

#include <boost/asio.hpp>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using error_code = boost::system::error_code;

#endif /* NET_H */
