#pragma once

#include "Common.h"

#include <random>

#include "DrawableBase.h"

class Sheet : public DrawableBase<Sheet>
{
public:
  Sheet(Graphics& gfx);

  void Update(float dt) noexcept override;

  DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
  float m_vx;
  float m_vy;
  float m_vz;

  float m_roll;
  float m_pitch;
  float m_yaw;
};
