#include "VertexLayout.h"

VertexLayout::Element::Element(ElementType type, size_t offset) :
  m_type(type),
  m_offset(offset)
{
}
