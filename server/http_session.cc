#include "http_session.h"
#include <iostream>
#include <utility>
#include "net.h"
#include "ws_session.h"

HttpSession::HttpSession(
  tcp::socket socket,
  std::function<void(HttpSession*)> unregister) :
  socket_{std::move(socket)},
  unregister_{unregister},
  wss_{nullptr} {
}

void HttpSession::run() {
  http::async_read(socket_, buffer_, req_,
    [this](error_code ec, std::size_t bytes) {
      this->on_read(ec, bytes);
    });
}

void HttpSession::on_read(error_code ec, std::size_t) {
  if (ec == http::error::end_of_stream) {
    socket_.shutdown(tcp::socket::shutdown_send, ec); // client closed
  } else if (ec) {
    std::cerr << "HttpSession::" << __func__ << " faild, ec=" << ec << '\n';
  } else if (websocket::is_upgrade(req_)) {
    std::cerr << "new websocket_session\n";
    wss_ = new WebSocketSession(std::move(socket_));
    wss_->run(std::move(req_));
  } else { // response
    ;
  }
}

