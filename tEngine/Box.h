#pragma once

#include "Common.h"

#include "DrawableBase.h"

class Box : public DrawableBase<Box>
{
public:
  Box(Graphics& gfx, DirectX::XMFLOAT3 material);

  void Update(float dt) noexcept override;

  DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
  float m_vx;
  float m_vy;
  float m_vz;

  float m_roll;
  float m_pitch;
  float m_yaw;

  DirectX::XMFLOAT3X3 m_model;
};
