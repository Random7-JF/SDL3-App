#include "game/game.h"

int main(int argc, char *argv[]) {
  Game game(1280, 720, 640, 360);
  game.Init();
  game.Run();
}
