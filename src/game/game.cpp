#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <fstream>
#include <sstream>
#include <string>

#include "asset.h"
#include "game.h"
#include "shader.h"

#define ASSERT(x) if (!(x)) __asm__ volatile("int3");
#define GLCall(x) GLClearError();\
  x;\
  ASSERT(GLLogcall(#x, __FILE__, __LINE__))

static void GLClearError() 
{
   while(glGetError() != GL_NO_ERROR);    
}

static bool GLLogcall(const char* function, const char* file, int line) 
{
  while(GLenum error = glGetError())
  {
    SDL_Log("OpenGL Error: %d - Function:%s File:%s Line:%d", error, function, file, line);
    return false;
  }
  return true;
}

bool Game::Init()
{
  bool initialized = false;
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Error Initializing SDL3", nullptr);
    return initialized;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  

  m_state.window =
      SDL_CreateWindow("SDL", m_state.windowWidth, m_state.windowHeight,
                       SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!m_state.window)
  {
    SDL_Log("Error with Create Window: %s", SDL_GetError());
    return initialized;
  }

  m_state.glcontext = SDL_GL_CreateContext(m_state.window);
  if (!m_state.glcontext)
  {
    SDL_Log("Error with Create Context: %s", SDL_GetError());
    return initialized;
  }

  SDL_GL_MakeCurrent(m_state.window, m_state.glcontext);
  SDL_GL_SetSwapInterval(1);

  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK)
  {
    SDL_Log("Glew Error: %s, %d", glewGetErrorString(glewError), glewError);
    return initialized;
  }

  glViewport(0, 0, m_state.windowWidth, m_state.windowHeight);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  initialized = true;
  return initialized;
}

void Game::Run()
{
  bool running = true;
  SDL_Log("running...");

  // data to go to the gpu
  float positions[] = {
      -0.5f, -0.5f, // 0
      0.5f, -0.5f,  // 1
      0.5f, 0.5f,   // 2
      -0.5f, 0.5f}; // 3
  unsigned int indices[] = {
      0, 1, 2,
      2, 3, 0};

  // create vertex attrib object VAO
  unsigned int vao;
  GLCall(glGenVertexArrays(1, &vao));
  GLCall(glBindVertexArray(vao));

  // create vertex buffer object VBO
  unsigned int vbo;
  GLCall(glGenBuffers(1, &vbo));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions,
               GL_STATIC_DRAW));

  // create indicies buffer object IBO
  unsigned int ibo;
  GLCall(glGenBuffers(1, &ibo));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices,GL_STATIC_DRAW));

  GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 2, 0));
  GLCall(glEnableVertexAttribArray(0));

  // get and compile shader from file
  ShaderProgramSource source = ParseShader("data/res/Basic.shader");
  unsigned int shader =
      CreateShader(source.VertexSource, source.FragmentSource);
  glUseProgram(shader);

  GLCall(int uniformLocation = glGetUniformLocation(shader, "u_Color"));
  ASSERT(uniformLocation != -1);
  
  float r = 0.0f;
  float increment = 0.005f;
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
        glViewport(0, 0, m_state.windowWidth, m_state.windowHeight);
        break;
      }
      }
    } // end of event loop

    // OpenGL
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

    // draw calls
    glBindVertexArray(vao);
    GLCall(glUniform4f(uniformLocation, r, 0.2f, 0.3f, 1.0f));
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    if (r > 1.0f)
      increment = -0.005f;
    else if (r < 0.0f)
      increment = 0.005f;

    r += increment;

    SDL_GL_SwapWindow(m_state.window);

  } // end of running loop
  glDeleteProgram(shader);
  SDL_DestroyRenderer(m_state.renderer);
  SDL_DestroyWindow(m_state.window);
}

void Game::DrawTriangle() {}
