#include <iostream>
#include <QApplication>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QString>
#include <QDebug>
#include <fmt/core.h>
#include "client.h"
#include "ui.h"

int main(int argc, char *argv[])
{
  using namespace Qt::Literals::StringLiterals;
  int rc =  0;
  std::cout << fmt::format("argc={} argv[0]={}\n", argc, argv[0]);
  QApplication a(argc, argv);
  qDebug() << "Main stream-shift to qDebug\n";
  int port = 9090;
  QUrl url;
  QString scheme(u"ws"_s);
  url.setScheme(scheme);
  url.setHost("localhost");
  url.setPort(port);
  UI ui;
  Client client(ui, url);
  QObject::connect(&client, &Client::Closed, &a, &QCoreApplication::quit);
  ui.SetClubRefresh([&client]() { client.ClubRefresh(); });
  ui.SetNewTableFunc(
    [&client](
        const std::string &table_name,
        const std::string &table_password,
        const std::string &owner_password) -> int {
      return client.NewTable(table_name, table_password, owner_password);
    });
  rc = a.exec();
  return rc;
}
