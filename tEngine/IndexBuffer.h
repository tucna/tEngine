#pragma once

#include <vector>

#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
  IndexBuffer(Graphics& gfx, const std::vector<uint16_t>& indices);

  void Bind(Graphics& gfx) noexcept override;

  UINT GetCount() const noexcept { return m_count; }

private:
  UINT m_count;

  Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};

