// -*- c++ -*-
#if !defined(WS_SESSION_H)
#define WS_SESSION_H 1

#include <functional>
#include <queue>
#include <string>
#include "net.h"
#include "beast.h"

class WebSocketSession {
 public:
  using report_message_t =
    std::function<void(WebSocketSession*, const std::string&)>;
  WebSocketSession(tcp::socket socket, report_message_t report_message);
  void run(http::request<http::string_body> req);
  void send(const std::string &s);
 private:
  void on_accept(error_code ec);
  void read_next();
  void on_read(error_code ec, std::size_t);
  void write_next();
  void on_write(error_code ec, std::size_t);
  beast::flat_buffer buffer_;
  websocket::stream<tcp::socket> socket_;
  std::queue<std::string> queue_;
  report_message_t report_message_;
};
  
#endif /* WS_SESSION_H */
