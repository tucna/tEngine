#pragma once

#include "Common.h"

#include "Graphics.h"

class Camera
{
public:
  Camera() noexcept;

  void Rotate(float dx, float dy) noexcept;
  void Translate(DirectX::XMFLOAT3 translation) noexcept;
  void Reset() noexcept;

  DirectX::XMMATRIX GetMatrix() const noexcept;

private:
  static constexpr float travelSpeed = 12.0f;
  static constexpr float rotationSpeed = 0.004f;

  DirectX::XMFLOAT3 m_position;

  float m_pitch;
  float m_yaw;
};

