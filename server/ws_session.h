// -*- c++ -*-
#if !defined(WS_SESSION_H)
#define WS_SESSION_H 1

#include "net.h"
#include "beast.h"

class WebSocketSession {
 public:
  WebSocketSession(tcp::socket socket);
  void run(http::request<http::string_body> req);
 private:
  tcp::socket socket_;
};
  
#endif /* WS_SESSION_H */
