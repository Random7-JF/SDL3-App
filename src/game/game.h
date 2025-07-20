#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
struct State
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  int windowWidth, windowHeight;
  int gameWidth, gameHeight;
};

class Game
{
private:
  State m_state;

public:
  Game(int window_width, int window_height, int game_width, int game_height)
      : m_state{
            nullptr,
            nullptr,
            window_width,
            window_height,
            game_width,
            game_height
        } { }

  bool Init();
  void Run();
};
