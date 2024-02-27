#include "ws_session.h"
#include <functional>
#include <iostream>
#include <utility>
#include "beast.h"
#include "utils.h"

WebSocketSession::WebSocketSession(tcp::socket socket) :
  socket_{std::move(socket)} {
}

void WebSocketSession::run(http::request<http::string_body> req) {
  socket_.async_accept(req, std::bind(
    &WebSocketSession::on_accept,
    this,
    std::placeholders::_1));
}

void WebSocketSession::on_accept(error_code ec) {
  if (ec) {
    std::cerr << funcname() << " failed, ec=" << ec << '\n';
  } else {
    read_next();
  }
}

void WebSocketSession::read_next() {
}
