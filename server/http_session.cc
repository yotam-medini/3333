#include "http_session.h"
#include <iostream>
#include <string>
#include <utility>
#include "net.h"
#include "utils.h"
#include "ws_session.h"

HttpSession::HttpSession(
  tcp::socket socket,
  std::function<void(HttpSession*, WebSocketSession*)> ws_register,
  std::function<void(HttpSession*)> unregister,
  WebSocketSession::report_message_t report_message) :
  socket_{std::move(socket)},
  ws_register_{ws_register},
  unregister_{unregister},
  report_message_{report_message},
  wss_{nullptr} {
}

HttpSession::~HttpSession() {
  unregister_(this);
  delete wss_;
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
    std::cerr << funcname() << " faild, ec=" << ec << '\n';
  } else if (websocket::is_upgrade(req_)) {
    std::cerr << "new websocket_session\n";
    wss_ = new WebSocketSession(std::move(socket_), report_message_);
    wss_->run(std::move(req_));
  } else { // response
    respond();
  }
}

void HttpSession::on_write(error_code ec, std::size_t, bool close) {
  if (ec) {
    std::cerr << funcname() << " fail ec=" << ec << '\n';
  } else if (close) {
    std::cerr << "HttpSession:: close\n";
  } else {
    req_ = {};
    run();
  }
}

void HttpSession::respond() {
  http::response<http::string_body>
    response{http::status::not_found, req_.version()};
  response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  response.set(http::field::content_type, "text/html");
  response.keep_alive(req_.keep_alive());
  response.body() = "resource not found.";
  response.prepare_payload();
  
  using response_type = typename std::decay<decltype(response)>::type;
  auto sp = std::make_shared<response_type>(
    std::forward<decltype(response)>(response));

  http::async_write(socket_, *sp,
    [this, sp](error_code ec, std::size_t bytes) {
      this->on_write(ec, bytes, sp->need_eof()); 
    });
}
