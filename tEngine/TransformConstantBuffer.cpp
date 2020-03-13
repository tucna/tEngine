#include "TransformConstantBuffer.h"

std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::Transformations>> TransformConstantBuffer::m_Vcbuf;

TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, const Drawable& parent, UINT slot) :
  m_parent(parent)
{
  if (!m_Vcbuf)
    m_Vcbuf = std::make_unique<VertexConstantBuffer<Transformations>>(gfx, slot);
}

void TransformConstantBuffer::Bind(Graphics& gfx) noexcept
{
  const auto model = m_parent.GetTransformXM();

  const Transformations tf =
  {
    DirectX::XMMatrixTranspose(model),
    DirectX::XMMatrixTranspose(model * gfx.GetViewMatrix() * gfx.GetProjectionMatrix())
  };

  m_Vcbuf->Update(gfx, tf);
  m_Vcbuf->Bind(gfx);
}
