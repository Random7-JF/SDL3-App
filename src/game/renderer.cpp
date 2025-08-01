#include <SDL3/SDL.h>
#include "renderer.h"

void GLClearError() 
{
   while(glGetError() != GL_NO_ERROR);    
}

bool GLLogcall(const char* function, const char* file, int line) 
{
  while(GLenum error = glGetError())
  {
    SDL_Log("OpenGL Error: %d - Function:%s File:%s Line:%d", error, function, file, line);
    return false;
  }
  return true;
}