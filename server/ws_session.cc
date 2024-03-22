#include "ws_session.h"
#include <functional>
#include <iostream>
#include <utility>
#include "beast.h"
#include "utils.h"

WebSocketSession::WebSocketSession(
  tcp::socket socket,
  report_message_t report_message) :
  socket_{std::move(socket)},
  report_message_{report_message} {
}

void WebSocketSession::run(http::request<http::string_body> req) {
  socket_.async_accept(req, std::bind(
    &WebSocketSession::on_accept,
    this,
    std::placeholders::_1));
}

void WebSocketSession::send(const std::string &s) {
  queue_.push(s);
  if (queue_.size() == 1) { // not yet writing
    write_next();
  }
}

void WebSocketSession::on_accept(error_code ec) {
  if (ec) {
    std::cerr << FuncName() << " failed, ec=" << ec << '\n';
  } else {
    read_next();
  }
}

void WebSocketSession::read_next() {
  socket_.async_read(
    buffer_,
    [this](error_code ec, std::size_t bytes) {
      this->on_read(ec, bytes);
    });
}

void WebSocketSession::on_read(error_code ec, std::size_t) {
  if (ec) { 
    std::cerr << FuncName() << " failed, ec=" << ec << '\n';
  } else {
    // state_->send(beast::buffers_to_string(buffer_.data()));
    const std::string message{beast::buffers_to_string(buffer_.data())};
    report_message_(this, message);
    buffer_.consume(buffer_.size()); // clear
    read_next();
  }
}

void WebSocketSession::write_next() {
  socket_.async_write(
    net::buffer(queue_.front()),
    [this](error_code ec, std::size_t bytes) {
      this->on_write(ec, bytes);
    });
}

void WebSocketSession::on_write(error_code ec, std::size_t)
{
  if (ec) {
    std::cerr << FuncName() << " failed, ec=" << ec << '\n';
  } else {
    queue_.pop();
    if (!queue_.empty()) {
      write_next();
    }
  }
}
