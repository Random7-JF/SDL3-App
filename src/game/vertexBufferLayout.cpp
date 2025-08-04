#include "vertexBufferLayout.h"

  template <>
  void VertexBufferLayout::Push<float>(unsigned int count)
  {
	VertexBufferElement element = {GL_FLOAT, count, GL_FALSE};
    m_elements.push_back(element);
    m_stride += count * element.GetSizeOfType(GL_FLOAT);
  }
  template <>
  void VertexBufferLayout::Push<unsigned int>(unsigned int  count)
  {
 	VertexBufferElement element = {GL_UNSIGNED_INT, count, GL_FALSE};
    m_elements.push_back(element);
    m_stride = count * element.GetSizeOfType(GL_UNSIGNED_INT);
  }
  template <>
  void VertexBufferLayout::Push<unsigned char>(unsigned int  count)
  {
    VertexBufferElement element = {GL_UNSIGNED_BYTE, count, GL_TRUE};
    m_elements.push_back(element);
    m_stride = count * element.GetSizeOfType(GL_UNSIGNED_BYTE);
  }