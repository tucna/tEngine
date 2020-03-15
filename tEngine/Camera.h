#pragma once

#include "Common.h"

#include "Graphics.h"

class Camera
{
public:
  Camera() noexcept;

  DirectX::XMMATRIX GetMatrix() const noexcept;
  void Reset() noexcept;

private:
  DirectX::XMFLOAT3 m_position;

  float m_pitch;
  float m_yaw;
};

