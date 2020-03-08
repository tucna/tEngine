#pragma once

#include "Common.h"

class VertexLayout
{
public:
  enum ElementType
  {
    Position3D,
  };

  template<ElementType> struct Map;
  template<> struct Map<Position3D>
  {
    using SysType = DirectX::XMFLOAT3;
    static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    static constexpr const char* semantic = "Position";
    static constexpr const char* code = "P3";
  };

  class Element
  {
  public:
    Element(ElementType type, size_t offset);

  private:
    template<ElementType type>
    static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc() noexcept
    {
      return { Map<type>::semantic, 0, Map<type>::dxgiFormat, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    }

    ElementType m_type;
    size_t m_offset;
  };

private:
};
