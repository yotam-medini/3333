#include "game.h"

Game::Game() {
}

void Game::Reset() {
  active_ = false;
  tstate_ = -1;
  gstate_ = -1;
  for (Player &player: players_) {
    player.reset();
  }
}
