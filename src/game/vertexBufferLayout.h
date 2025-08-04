#pragma once

#include "game/renderer.h"
#include <vector>
struct VertexBufferElement {
  unsigned int type;
  unsigned int count;
  bool normalized;
};

class VertexBufferLayout {
private:
  std::vector<VertexBufferElement> m_elements;

public:
  VertexBufferLayout();

  template <typename T> void Push(int count) { static_assert(false); }

  template <> void Push<float>(int count) {
    m_elements.push_back({GL_FLOAT, count, false});
  }
};
