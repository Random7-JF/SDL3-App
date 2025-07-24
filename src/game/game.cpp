#include <SDL3/SDL.h>
#include <GL/glew.h>

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
      SDL_CreateWindow("SDL", m_state.windowWidth, m_state.windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!m_state.window)
  {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Error creating window", m_state.window);
    return initialized;
  }

  m_state.glcontext = SDL_GL_CreateContext(m_state.window);
  if (!m_state.glcontext)
  {
    SDL_Log(SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Error creating gl context", m_state.window);
    return initialized;
  }

  SDL_GL_MakeCurrent(m_state.window, m_state.glcontext);

  GLenum glewError = glewInit();
  if (glewError != GLEW_OK)
  {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Error glew init", m_state.window);
    SDL_Log("%s", glewError);
    return initialized;
  }
  glViewport(0, 0, m_state.windowWidth, m_state.windowHeight); // Initial viewport
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);                        // Dark teal background

  initialized = true;

  return initialized;
}

void Game::Run()
{
  SDL_Log("running...");
  SDL_SetRenderLogicalPresentation(m_state.renderer, m_state.gameWidth, m_state.gameHeight,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  bool running = true;

  float positions[6] = {
      -0.5f, -0.5f,
      0.0f, 0.5f,
      0.5f, -0.5f};

  // create gl (vertex)buffer
  unsigned int buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 2, 0);

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

    // OpenGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(m_state.window);

  } // end of running loop
  SDL_DestroyRenderer(m_state.renderer);
  SDL_DestroyWindow(m_state.window);
}

void Game::DrawTriangle()
{
}