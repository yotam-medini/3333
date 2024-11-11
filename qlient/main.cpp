#include <iostream>
#include <QApplication>
#include <QtCore/QString>
#include <QDebug>
#include <fmt/core.h>
#include "client.h"
#include "game.h"
#include "ui.h"

void SetFunctions(Client &client, UI& ui) {
  ui.SetClubRefresh([&client]() { client.ClubRefresh(); });
  ui.SetNewTableFunc(
    [&client](
        const std::string &table_name,
        const std::string &table_password,
        const std::string &owner_password) -> int {
      return client.NewTable(table_name, table_password, owner_password);
    });
  ui.SetNewGameFunc([&client]() -> int { return client.NewGame(); });
  ui.SetAdd3NoMoreFunc([&client]() -> int { return client.Add3NoMore(); });
  ui.SetTry3Func([&client](const std::vector<unsigned> &ca) -> int {
    return client.Try3(ca);
  });
}

int main(int argc, char *argv[])
{
  int rc =  0;
  std::cout << fmt::format("argc={} argv[0]={}\n", argc, argv[0]);
  QApplication a(argc, argv);
  const std::string url{argc > 1 ? argv[1] : ""};
  qDebug() << fmt::format("command line url={}", url);
  Game game;
  UI ui(game);
  Client client(ui, game, url);
  QObject::connect(&client, &Client::Closed, &a, &QCoreApplication::quit);
  SetFunctions(client, ui);
  rc = a.exec();
  qDebug() << fmt::format("QApplication exec exit rc={}", rc);
  return rc;
}
