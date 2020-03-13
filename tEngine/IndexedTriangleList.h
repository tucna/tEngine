#pragma once

#include "Common.h"

#include <vector>

template<class T>
class IndexedTriangleList
{
public:
  IndexedTriangleList() = default;
  IndexedTriangleList(std::vector<T> verts_in, std::vector<uint16_t> indices_in) :
    m_vertices(std::move(verts_in)),
    m_indices(std::move(indices_in))
  {
    assert(m_vertices.size() > 2);
    assert(m_indices.size() % 3 == 0);
  }

  void Transform(DirectX::FXMMATRIX matrix)
  {
    for (auto& v : m_vertices)
    {
      const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);

      DirectX::XMStoreFloat3(&v.pos, DirectX::XMVector3Transform(pos, matrix));
    }
  }

  void SetNormalsIndependentFlat() noexcept
  {
    assert(m_indices.size() % 3 == 0 && m_indices.size() > 0);

    for (size_t i = 0; i < m_indices.size(); i += 3)
    {
      auto& v0 = m_vertices[m_indices[i + 0]];
      auto& v1 = m_vertices[m_indices[i + 1]];
      auto& v2 = m_vertices[m_indices[i + 2]];

      const auto p0 = XMLoadFloat3(&v0.pos);
      const auto p1 = XMLoadFloat3(&v1.pos);
      const auto p2 = XMLoadFloat3(&v2.pos);

      const auto n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross((p1 - p0), (p2 - p0)));

      DirectX::XMStoreFloat3(&v0.n, n);
      DirectX::XMStoreFloat3(&v1.n, n);
      DirectX::XMStoreFloat3(&v2.n, n);
    }
  }

  std::vector<T> m_vertices;
  std::vector<uint16_t> m_indices;
};
