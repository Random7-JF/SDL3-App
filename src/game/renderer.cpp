#include <SDL3/SDL.h>
#include "renderer.h"

void GLClearError()
{
  while (glGetError() != GL_NO_ERROR)
    ;
}

bool GLLogcall(const char *function, const char *file, int line)
{
  while (GLenum error = glGetError())
  {
    SDL_Log("OpenGL Error: %d - Function:%s File:%s Line:%d", error, function, file, line);
    return false;
  }
  return true;
}

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const
{
  shader.Bind();
  va.Bind();
  ib.Bind();
  GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Clear() const
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
