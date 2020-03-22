#include "TransformConstantBuffer.h"

std::unique_ptr<Bind::VertexConstantBuffer<Bind::TransformConstantBuffer::Transformations>> Bind::TransformConstantBuffer::m_Vcbuf;

Bind::TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, const Drawable& parent, UINT slot) :
  m_parent(parent)
{
  if (!m_Vcbuf)
    m_Vcbuf = std::make_unique<Bind::VertexConstantBuffer<Transformations>>(gfx, slot);
}

void Bind::TransformConstantBuffer::Bind(Graphics& gfx) noexcept
{
  UpdateBindImpl(gfx, GetTransforms(gfx));
}

void Bind::TransformConstantBuffer::UpdateBindImpl(Graphics& gfx, const Transformations& tf) noexcept
{
  m_Vcbuf->Update(gfx, tf);
  m_Vcbuf->Bind(gfx);
}

Bind::TransformConstantBuffer::Transformations Bind::TransformConstantBuffer::GetTransforms(Graphics& gfx) noexcept
{
  const auto modelView = m_parent.GetTransformXM() * gfx.GetViewMatrix();

  return {
    DirectX::XMMatrixTranspose(modelView),
    DirectX::XMMatrixTranspose(modelView * gfx.GetProjectionMatrix())
  };
}
