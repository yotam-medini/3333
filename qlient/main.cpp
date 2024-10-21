#include <iostream>
#include <QtCore/QCoreApplication>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QString>
#include "client.h"

int main(int argc, char *argv[])
{
  using namespace Qt::Literals::StringLiterals;
  int rc =  0;
  std::cout << "argc="<<argc << " argv[0]="<<argv[0] << '\n';
  QCoreApplication a(argc, argv);
  int port = 9090;
  QUrl url;
  QString scheme(u"ws"_s);
  url.setScheme(scheme);
  url.setHost("localhost");
  url.setPort(port);
  Client client(url);
  QObject::connect(&client, &Client::Closed, &a, &QCoreApplication::quit);
  rc = a.exec();
  return rc;
}
