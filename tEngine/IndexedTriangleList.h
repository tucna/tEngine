#pragma once

#include "Common.h"

#include <vector>

template<class T>
class IndexedTriangleList
{
public:
  IndexedTriangleList() = default;
  IndexedTriangleList(std::vector<T> verts_in, std::vector<uint16_t> indices_in);

  void Transform(DirectX::FXMMATRIX matrix);
  void SetNormalsIndependentFlat() noexcept;

  std::vector<T> m_vertices;
  std::vector<uint16_t> m_indices;
};
