#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "game.h"
#include "indexBuffer.h"
#include "renderer.h"
#include "shader.h"
#include "vertexBuffer.h"
#include "vertexArray.h"
#include "vertexBufferLayout.h"
#include "texture.h"

bool Game::Init() {
  bool initialized = false;
  if (!SDL_Init(SDL_INIT_VIDEO)) {
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
  if (!m_state.window) {
    SDL_Log("Error with Create Window: %s", SDL_GetError());
    return initialized;
  }

  m_state.glcontext = SDL_GL_CreateContext(m_state.window);
  if (!m_state.glcontext) {
    SDL_Log("Error with Create Context: %s", SDL_GetError());
    return initialized;
  }

  SDL_GL_MakeCurrent(m_state.window, m_state.glcontext);
  SDL_GL_SetSwapInterval(1);

  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    SDL_Log("Glew Error: %s, %d", glewGetErrorString(glewError), glewError);
    return initialized;
  }

  glViewport(0, 0, m_state.windowWidth, m_state.windowHeight);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  initialized = true;
  return initialized;
}

void Game::Run() {
  bool running = true;
  SDL_Log("running...");

  // data to go to the gpu
float vertices[] = {
    // Index 0: Bottom-left
    -0.5f, -0.5f,  0.0f, 0.0f,
    // Index 1: Bottom-right
     0.5f, -0.5f,  1.0f, 0.0f,
    // Index 2: Top-right
     0.5f,  0.5f, 1.0f, 1.0f,
    // Index 3: Top-left
    -0.5f,  0.5f, 0.0f, 1.0f
};

// 6 indices to form two triangles from the 4 vertices
unsigned int indices[] = {
    0, 1, 2, // First triangle: BL, BR, TR
    2, 3, 0  // Second triangle: TR, TL, BL (Note: Winding order consistent)
};

  // create vertex attrib object VAO
  VertexArray va;  
  // create vertex buffer object VBO
  VertexBuffer vb(vertices, 4 * 4 * sizeof(float));
  // create and save the layout.
  VertexBufferLayout layout;
  layout.Push<float>(2);// pos
  layout.Push<float>(2);// tex
  va.AddBuffer(vb, layout);
  // create indicies buffer object IBO
  IndexBuffer ib(indices, 6);
  
  // get and compile shader from file
  Shader shader("data/res/Basic.shader");
  shader.Bind();
  //shader.SetUniform4f("u_Color", 0.0f, 0.2f, 0.3f, 1.0f);
  // create renderer from class
  Renderer renderer;
  Texture texture("data/textures/logo.png");
  texture.Bind();
  shader.SetUniform1i("u_Texture", 0);
   
  // projection matrix
  glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
  shader.SetUniformMat4f("u_MVP", proj);

  // start of the running loop
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
        m_state.windowWidth = event.window.data1;
        m_state.windowHeight = event.window.data2;
        glViewport(0, 0, m_state.windowWidth, m_state.windowHeight);
        break;
      }
      }
    } // end of event loop

    renderer.Clear();    
    shader.Bind();
    //shader.SetUniform4f("u_Color", r, 0.2f, 0.3f, 1.0f);
    renderer.Draw(va,ib,shader);

    SDL_GL_SwapWindow(m_state.window);

  } // end of running loop
  SDL_DestroyRenderer(m_state.renderer);
  SDL_DestroyWindow(m_state.window);
}

void Game::DrawTriangle() {}
