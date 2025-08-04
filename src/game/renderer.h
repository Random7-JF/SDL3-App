#pragma once
#include <GL/glew.h>
#include "vertexArray.h"
#include "indexBuffer.h"
#include "shader.h"


#define ASSERT(x) if (!(x)) __asm__ volatile("int3");
#define GLCall(x) GLClearError();\
  x;\
  ASSERT(GLLogcall(#x, __FILE__, __LINE__))


void GLClearError();
bool GLLogcall(const char* function, const char* file, int line);

class Renderer {
  public:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;
};