#include "ws_session.h"
#include <functional>
#include <utility>
#include "beast.h"

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
}
