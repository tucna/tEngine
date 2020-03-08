#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<uint16_t>& indices) :
  m_count((UINT)indices.size())
{
  D3D11_BUFFER_DESC ibd = {};
  ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  ibd.Usage = D3D11_USAGE_DEFAULT;
  ibd.CPUAccessFlags = 0u;
  ibd.MiscFlags = 0u;
  ibd.ByteWidth = UINT(m_count * sizeof(uint16_t));
  ibd.StructureByteStride = sizeof(uint16_t);

  D3D11_SUBRESOURCE_DATA isd = {};
  isd.pSysMem = indices.data();

  CHECK_HR(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &m_indexBuffer));
}

void IndexBuffer::Bind(Graphics & gfx) noexcept
{
  GetContext(gfx)->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}
