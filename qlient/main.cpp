#include <iostream>
#include <QApplication>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QString>
#include <QDebug>
#include "client.h"
#include "ui.h"

int main(int argc, char *argv[])
{
  using namespace Qt::Literals::StringLiterals;
  int rc =  0;
  std::cout << "argc="<<argc << " argv[0]="<<argv[0] << '\n';
  QApplication a(argc, argv);
  qDebug() << "Main stream-shift to qDebug\n";
  int port = 9090;
  QUrl url;
  QString scheme(u"ws"_s);
  url.setScheme(scheme);
  url.setHost("localhost");
  url.setPort(port);
  Client client(url);
  QObject::connect(&client, &Client::Closed, &a, &QCoreApplication::quit);
  UI ui;
  ui.run();
  rc = a.exec();
  return rc;
}
