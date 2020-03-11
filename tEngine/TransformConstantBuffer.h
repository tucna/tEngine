#pragma once

#include "Common.h"

#include "Bindable.h"
#include "ConstantBuffers.h"
#include "Drawable.h"

class TransformConstantBuffer : public Bindable
{
public:
  TransformConstantBuffer(Graphics& gfx, const Drawable& parent, UINT slot = 0u);

  void Bind(Graphics& gfx) noexcept override;

private:
  struct Transformations
  {
    DirectX::XMMATRIX modelViewProj;
  };

  static std::unique_ptr<VertexConstantBuffer<Transformations>> m_Vcbuf;

  const Drawable& m_parent;
};
