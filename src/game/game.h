#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

class Game {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  int windowWidth, windowHeight;
  int gameWidth, gameHeight;

public:
  Game(int window_width, int window_height, int game_width, int game_height)
      : windowWidth(window_width), windowHeight(window_height),
        gameWidth(game_width), gameHeight(game_height) {}

  bool Init();
  void Run();
};
