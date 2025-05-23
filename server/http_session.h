// -*- c++ -*-
#pragma once

#include "net.h"
#include "beast.h"

class WebSocketSession;

class HttpSession {
 public:
  HttpSession(
    tcp::socket socket,
    std::function<void(HttpSession*, WebSocketSession*)> ws_register,
    std::function<void(HttpSession*)> unregister,
    std::function<void(WebSocketSession*, const std::string&)> report_message);
  ~HttpSession();
  void run();
  WebSocketSession *get_websocket_server() const { return wss_; }
 private:
  void on_read(error_code ec, std::size_t);
  void on_write(error_code ec, std::size_t, bool close);
  void respond();
  void DeleteMe();
  tcp::socket socket_;
  beast::flat_buffer buffer_;
  http::request<http::string_body> req_;
  std::function<void(HttpSession*, WebSocketSession*)> ws_register_;
  std::function<void(HttpSession*)> unregister_;
  std::function<void(WebSocketSession*, const std::string&)> report_message_;
  WebSocketSession *wss_;
};
