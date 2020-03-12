#pragma once

#include "Common.h"

#include "DrawableBase.h"

class Box : public DrawableBase<Box>
{
public:
  Box(Graphics& gfx/*, DirectX::XMFLOAT3 material TUCNA*/);

  void Update(float dt) noexcept override;

  DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
  // positional
  float r = 0.0f;
  float roll = 0.0f;
  float pitch = 0.0f;
  float yaw = 0.0f;

  DirectX::XMFLOAT3X3 m_model;
};
