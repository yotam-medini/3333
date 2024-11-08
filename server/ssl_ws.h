// -*- c++ -*-
#pragma once

#include "net.h"
#include "beast.h"

using ssl_ws = websocket::stream<net::ssl::stream<net::ip::tcp::socket>>;
