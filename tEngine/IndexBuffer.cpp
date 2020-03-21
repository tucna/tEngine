#include "Codex.h"
#include "IndexBuffer.h"

Bind::IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices) :
  IndexBuffer(gfx, "?", indices)
{
}

Bind::IndexBuffer::IndexBuffer(Graphics & gfx, std::string tag, const std::vector<unsigned short>& indices) :
  m_tag(tag),
  m_count((UINT)indices.size())
{
  D3D11_BUFFER_DESC ibd = {};
  ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  ibd.Usage = D3D11_USAGE_DEFAULT;
  ibd.CPUAccessFlags = 0u;
  ibd.MiscFlags = 0u;
  ibd.ByteWidth = UINT(m_count * sizeof(unsigned short));
  ibd.StructureByteStride = sizeof(unsigned short);

  D3D11_SUBRESOURCE_DATA isd = {};
  isd.pSysMem = indices.data();

  CHECK_HR(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &m_indexBuffer));
}

void Bind::IndexBuffer::Bind(Graphics & gfx) noexcept
{
  GetContext(gfx)->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

std::string Bind::IndexBuffer::GetUID() const noexcept
{
  return GenerateUID_(m_tag);
}

std::shared_ptr<Bind::IndexBuffer> Bind::IndexBuffer::Resolve(Graphics & gfx, const std::string & tag, const std::vector<unsigned short>& indices)
{
  assert(tag != "?");

  return Codex::Resolve<Bind::IndexBuffer>(gfx, tag, indices);
}

std::string Bind::IndexBuffer::GenerateUID_(const std::string & tag)
{
  using namespace std::string_literals;

  return typeid(IndexBuffer).name() + "#"s + tag;
}
