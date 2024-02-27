// -*- c++ -*-
#if !defined(HTTP_SESSION_H)
#define HTTP_SESSION_H 1

#include "net.h"

class HttpSession {
 public:
  HttpSession(
    tcp::socket socket,
    std::function<void(HttpSession*)> unregister);
  void run();
};

#endif /* HTTP_SESSION_H */
