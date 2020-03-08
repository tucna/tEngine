#pragma once

#include <vector>

#include "Bindable.h"

class VertexBuffer : public Bindable
{
public:
  template<class V>
  VertexBuffer(Graphics& gfx, const std::vector<V>& vertices);

  void Bind(Graphics& gfx) noexcept override;

private:
  UINT m_stride;
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
};
