#include "game.h"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

bool Game::Init() {
  bool initialized = false;
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Error Initializing SDL3", nullptr);
    return initialized;
  }

  window =
      SDL_CreateWindow("SDL", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
  if (!window) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Error creating window", window);
    return initialized;
  }

  renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "error creating renderer", window);
    return initialized;
  }
  initialized = true;

  return initialized;
}

void Game::Run() {
  SDL_Log("running...");
  SDL_SetRenderLogicalPresentation(renderer, gameWidth, gameHeight,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  bool running = true;
  while (running) {
    SDL_Event event{0};
    // start of event loop
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT: {
        running = false;
        break;
      }
      case SDL_EVENT_WINDOW_RESIZED: {
        windowWidth = event.window.data1;
        windowHeight = event.window.data2;
        break;
      }
      }
    } // end of event loop
    SDL_SetRenderDrawColor(renderer, 28, 20, 10, 255);
    SDL_RenderClear(renderer);

    // swap buffers
    SDL_RenderPresent(renderer);
  } // end of running loop
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}
