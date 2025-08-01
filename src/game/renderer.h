#pragma once
#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __asm__ volatile("int3");
#define GLCall(x) GLClearError();\
  x;\
  ASSERT(GLLogcall(#x, __FILE__, __LINE__))


void GLClearError();
bool GLLogcall(const char* function, const char* file, int line);
