#pragma once

#include "Common.h"

#include "Bindable.h"
#include "ConstantBuffers.h"
#include "Drawable.h"

namespace Bind
{

class TransformConstantBuffer : public Bindable
{
public:
  TransformConstantBuffer(Graphics& gfx, const Drawable& parent, UINT slot = 0u);

  void Bind(Graphics& gfx) noexcept override;

  std::string GetUID() const noexcept override { return ""; /*TODO*/ }

private:
  struct Transformations
  {
    DirectX::XMMATRIX model;
    DirectX::XMMATRIX modelViewProj;
  };

  void UpdateBindImpl(Graphics& gfx, const Transformations& tf) noexcept;
  Transformations GetTransforms(Graphics& gfx) noexcept;

  static std::unique_ptr<Bind::VertexConstantBuffer<Transformations>> m_Vcbuf;

  const Drawable& m_parent;
};

}
