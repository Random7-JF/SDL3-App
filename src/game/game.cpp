#include <SDL3/SDL.h>

#include "game.h"
#include "asset.h"

bool Game::Init()
{
  bool initialized = false;
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Error Initializing SDL3", nullptr);
    return initialized;
  }

  m_state.window =
      SDL_CreateWindow("SDL", m_state.windowWidth, m_state.windowHeight, SDL_WINDOW_RESIZABLE);
  if (!m_state.window)
  {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Error creating window", m_state.window);
    return initialized;
  }

  m_state.renderer = SDL_CreateRenderer(m_state.window, NULL);
  if (!m_state.renderer)
  {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "error creating renderer", m_state.window);
    return initialized;
  }
  initialized = true;

  return initialized;
}

void Game::Run()
{
  SDL_Log("running...");
  SDL_SetRenderLogicalPresentation(m_state.renderer, m_state.gameWidth, m_state.gameHeight,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  bool running = true;

  Asset player(IMG_LoadTexture(m_state.renderer, "data/player.png"), SDL_SCALEMODE_NEAREST);

  // start of the running loop
  while (running)
  {
    SDL_Event event{0};
    // start of event loop
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_EVENT_QUIT:
      {
        running = false;
        break;
      }
      case SDL_EVENT_WINDOW_RESIZED:
      {
        m_state.windowWidth = event.window.data1;
        m_state.windowHeight = event.window.data2;
        break;
      }
      }
    } // end of event loop
    SDL_SetRenderDrawColor(m_state.renderer, 28, 20, 10, 255);
    SDL_RenderClear(m_state.renderer);

    SDL_FRect src{
        .x = 32 * 22,
        .y = 32,
        .w = 32,
        .h = 32};

    SDL_FRect dst{
        .x = 150,
        .y = 150,
        .w = 64,
        .h = 64};

    SDL_RenderTextureRotated(m_state.renderer, player.getTexture(), &src, &dst, 0, nullptr, SDL_FLIP_NONE);

    // swap buffers
    SDL_RenderPresent(m_state.renderer);
  } // end of running loop
  SDL_DestroyRenderer(m_state.renderer);
  SDL_DestroyWindow(m_state.window);
}
