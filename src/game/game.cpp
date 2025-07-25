#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <fstream>
#include <sstream>
#include <string>

#include "asset.h"
#include "game.h"

struct ShaderProgramSource {
  std::string VertexSource;
  std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string &filepath) {
  std::ifstream stream(filepath);
  enum class ShaderType { NONE = -1, VERTEX = -0, FRAGMENT = 1 };

  std::string line;
  std::stringstream ss[2];
  ShaderType type = ShaderType::NONE;

  while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos) {
        type = ShaderType::VERTEX;
      } else if (line.find("fragment") != std::string::npos) {
        type = ShaderType::FRAGMENT;
      }
    } else {
      ss[(int)type] << line << "\n";
    }
  }
  return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type,
                                  const std::string &source) {
  unsigned int id = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(id, 1, &src, nullptr); // read the docs on this.
  glCompileShader(id);
  if (type == GL_VERTEX_SHADER) {
    SDL_Log("--- Compiling Vertex Shader ---");
  } else {
    SDL_Log("--- Compiling Fragment Shader ---");
  }
  SDL_Log("%s", src);
  SDL_Log("-----------------------------");
  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);

  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    if (type == GL_VERTEX_SHADER) {
      SDL_Log("Failed to compile vertex shader: %s", message);
    } else {
      SDL_Log("Failed to compile frag shader: %s", message);
    }
    glDeleteShader(id);
    return 0;
  }

  return id;
}

static unsigned int CreateShader(const std::string &vertexShader,
                                 const std::string &fragmentShader) {
  unsigned int program = glCreateProgram();
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);

  // --- ADD LINKING ERROR CHECKING ---
  int isLinked;
  glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
  if (isLinked == GL_FALSE) {
    int maxLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    char *infoLog = (char *)alloca(
        maxLength * sizeof(char)); // Use alloca for stack allocation
    glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
    SDL_Log("Failed to link shader program: %s", infoLog);
    glDeleteProgram(program);
    glDeleteShader(vs); // Still delete attached shaders even if linking fails
    glDeleteShader(fs);
    return 0; // Indicate failure
  }
  // ------------------------------------
  glValidateProgram(program);

  // --- ADD VALIDATION ERROR CHECKING ---
  int isValid;
  glGetProgramiv(program, GL_VALIDATE_STATUS, &isValid);
  if (isValid == GL_FALSE) {
    int maxLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    char *infoLog = (char *)alloca(maxLength * sizeof(char));
    glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
    SDL_Log("Shader program validation failed: %s", infoLog);
    // Validation failure isn't always fatal, but often indicates a problem.
    // Depending on strictness, you might delete program here or proceed.
  }
  // ------------------------------------
  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

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
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // Important for 3D

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
  SDL_Log("running...");
  // SDL_SetRenderLogicalPresentation(m_state.renderer, m_state.gameWidth,
  // m_state.gameHeight,
  //                                  SDL_LOGICAL_PRESENTATION_LETTERBOX);
  bool running = true;

  float positions[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};

  unsigned int indices[] = {0, 1, 2, 2, 3, 0};

  // create vertex attrib object VAO
  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // create vertex buffer object VBO
  unsigned int vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions,
               GL_STATIC_DRAW);

  // create indicies buffer object
  unsigned int ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 2, 0);
  glEnableVertexAttribArray(0);

  ShaderProgramSource source = ParseShader("data/res/Basic.shader");
  SDL_Log("---Vertex---\n%s", source.VertexSource.c_str());
  SDL_Log("---Fragment---\n %s", source.FragmentSource.c_str());
  unsigned int shader =
      CreateShader(source.VertexSource, source.FragmentSource);
  glUseProgram(shader);

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

    // OpenGL
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

    // draw call
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    SDL_GL_SwapWindow(m_state.window);

  } // end of running loop
  glDeleteProgram(shader);
  SDL_DestroyRenderer(m_state.renderer);
  SDL_DestroyWindow(m_state.window);
}

void Game::DrawTriangle() {}
