#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "game.h"
#include "indexBuffer.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"
#include "vertexArray.h"
#include "vertexBuffer.h"
#include "vertexBufferLayout.h"

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
  GLCall(glEnable(GL_DEPTH_TEST));
  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  // GLCall(glDepthFunc(GL_LESS));
  // GLCall(glCullFace(GL_BACK));
  // GLCall(glFrontFace(GL_CCW));


  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK)
  {
    SDL_Log("Glew Error: %s, %d", glewGetErrorString(glewError), glewError);
    return initialized;
  }

  glViewport(0, 0, m_state.windowWidth, m_state.windowHeight);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  ImGui_ImplSDL3_InitForOpenGL(m_state.window, m_state.glcontext);
  ImGui_ImplOpenGL3_Init();

  initialized = true;
  return initialized;
}

void Game::Run()
{
  bool running = true;
  SDL_Log("running...");

  // data to go to the gpu
  // float vertices[] = {
  //   // X      Y      Z      U      V
  //   -0.5f, 0.0f,  0.5f, 0.0f, 0.0f, // Vertex 0: FL - (0,0)
  //   -0.5f, 0.0f, -0.5f, 1.0f, 0.0f, // Vertex 1: BL - (1,0)
  //    0.5f, 0.0f, -0.5f, 0.0f, 0.0f, // Vertex 2: BR - (0,0)
  //    0.5f, 0.0f,  0.5f, 1.0f, 0.0f, // Vertex 3: FR - (1,0)
  //    0.0f, 0.8f,  0.0f, 0.5f, 1.0f  // Vertex 4: Apex - (0.5,1)
  // };
  // unsigned int indices[] = {
  //   0,1,2,
  //   0,3,2,
  //   0,3,4,
  //   3,2,4,
  //   2,1,4,
  //   1,0,4
  // };
    float vertices[] = {
      -0.5f, 0.0f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.0f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.0f, -0.5f, 0.0f, 0.0f,
      0.5f, 0.0f, 0.5f, 1.0f, 0.0f,
      0.0f, 0.8f, 0.0f, 0.5f, 1.0f,

      -0.5f, 0.0f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.0f, -0.5f, 0.0f, 1.0f,
      0.5f, 0.0f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.0f, 0.5f, 1.0f, 0.0f};

  unsigned int indices[] = {
      0, 3, 4,
      3, 2, 4,
      2, 1, 4,
      1, 0, 4,

      5, 6, 7,
      5, 7, 8};

  // create vertex attrib object VAO
  VertexArray va;
  // create vertex buffer object VBO
  VertexBuffer vb(vertices, 9 * 5 * sizeof(float));
  // create and save the layout.
  VertexBufferLayout layout;
  layout.Push<float>(3); // pos
  layout.Push<float>(2); // tex
  va.AddBuffer(vb, layout);
  // create indicies buffer object IBO
  IndexBuffer ib(indices, 18);

  // get and compile shader from file
  Shader shader("data/res/Basic.shader");
  shader.Bind();
  // shader.SetUniform4f("u_Color", 0.0f, 0.2f, 0.3f, 1.0f);
  //  create renderer from class
  Renderer renderer;
  Texture texture("data/textures/brick.png");
  texture.Bind();

  float rotation = 0.0f;
  auto currentTime = SDL_GetPerformanceCounter();
  glm::vec3 cameraPos(0.0f,-0.5f, -2.0f);
  // start of the running loop
  while (running)
  {
    auto nowTime = SDL_GetPerformanceCounter();
    auto deltaTime = nowTime - currentTime;

    SDL_Event event{0};
    // start of event loop
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL3_ProcessEvent(&event); // Forward your event to backend
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

    if (deltaTime > 60) {
        rotation -= 0.05f;
    }
      // Model Matrix - model Pos
      glm::mat4 model = glm::mat4(1.0f); 
      // View Matrix - Camera Pos
      glm::mat4 view = glm::mat4(1.0f); 
      // Projection Matix - 3d effect.
      glm::mat4 proj = glm::mat4(1.0f);
      // final output
      model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f,1.0f,0.0f));
      view = glm::translate(view, cameraPos);
      proj = glm::perspective(glm::radians(45.0f), (float)(m_state.gameWidth/m_state.gameHeight), 0.1f, 100.0f);

      glm::mat4 mvp = proj* view * model;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    // ImGui::ShowDemoWindow();
    ImGui::SliderFloat3("Camera", &cameraPos.x,-5,5);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    renderer.Clear();
    shader.Bind();
    shader.SetUniformMat4f("u_MVP", mvp);
    renderer.Draw(va, ib, shader);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(m_state.window);
    currentTime = SDL_GetPerformanceCounter();

  } // end of running loop
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(m_state.renderer);
  SDL_DestroyWindow(m_state.window);
}

void Game::DrawTriangle() {}
