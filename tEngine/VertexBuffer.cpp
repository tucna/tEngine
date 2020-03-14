#include "VertexBuffer.h"

using namespace Bind;

VertexBuffer::VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf) :
  m_stride((UINT)vbuf.GetLayout().Size())
{
  D3D11_BUFFER_DESC bd = {};
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.CPUAccessFlags = 0u;
  bd.MiscFlags = 0u;
  bd.ByteWidth = UINT(vbuf.SizeBytes());
  bd.StructureByteStride = m_stride;
  D3D11_SUBRESOURCE_DATA sd = {};
  sd.pSysMem = vbuf.GetData();
  CHECK_HR(GetDevice(gfx)->CreateBuffer(&bd, &sd, &m_vertexBuffer));
}

void VertexBuffer::Bind(Graphics& gfx) noexcept
{
  const UINT offset = 0u;
  GetContext(gfx)->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &offset);
}
