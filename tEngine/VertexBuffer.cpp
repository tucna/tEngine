#include "Codex.h"
#include "VertexBuffer.h"

Bind::VertexBuffer::VertexBuffer(Graphics & gfx, const std::string & tag, const Dvtx::VertexBuffer & vbuf) :
  m_stride((UINT)vbuf.GetLayout().Size()),
  m_tag(tag)
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

Bind::VertexBuffer::VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf) :
  VertexBuffer(gfx, "?", vbuf)
{
}

void Bind::VertexBuffer::Bind(Graphics & gfx) noexcept
{
  const UINT offset = 0u;

  GetContext(gfx)->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &offset);
}

std::string Bind::VertexBuffer::GetUID() const noexcept
{
  return GenerateUID(m_tag);
}

std::shared_ptr<Bind::VertexBuffer> Bind::VertexBuffer::Resolve(Graphics & gfx, const std::string & tag, const Dvtx::VertexBuffer & vbuf)
{
  assert(tag != "?");

  return Codex::Resolve<Bind::VertexBuffer>(gfx, tag, vbuf);
}

std::string Bind::VertexBuffer::GenerateUID_(const std::string & tag)
{
  using namespace std::string_literals;

  return typeid(Bind::VertexBuffer).name() + "#"s + tag;
}
