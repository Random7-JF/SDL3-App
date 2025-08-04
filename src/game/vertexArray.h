#pragma once
#include "vertexBuffer.h"

class VertexBufferLayout;

class VertexArray
{
private:
  unsigned int m_renderID;

public:
  VertexArray();
  ~VertexArray();

  void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
  void Bind() const;
  void Unbind() const;
};
