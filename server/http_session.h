// -*- c++ -*-
#if !defined(HTTP_SESSION_H)
#define HTTP_SESSION_H 1

#include "net.h"
#include "beast.h"

class WebSocketSession;

class HttpSession {
 public:
  HttpSession(
    tcp::socket socket,
    std::function<void(HttpSession*)> unregister);
  ~HttpSession();
  void run();
 private:
  void on_read(error_code ec, std::size_t);
  void on_write(error_code ec, std::size_t, bool close);
  void respond();
  tcp::socket socket_;
  beast::flat_buffer buffer_;
  http::request<http::string_body> req_;
  std::function<void(HttpSession*)> unregister_;
  WebSocketSession *wss_;
};

#endif /* HTTP_SESSION_H */
