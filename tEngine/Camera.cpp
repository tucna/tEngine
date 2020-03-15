#include "Camera.h"

using namespace DirectX;

Camera::Camera() noexcept
{
  Reset();
}

XMMATRIX Camera::GetMatrix() const noexcept
{
  const XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

  const auto lookVector = XMVector3Transform(forwardBaseVector, XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f));
  const auto camPosition = XMLoadFloat3(&m_position);
  const auto camTarget = camPosition + lookVector;

  return XMMatrixLookAtLH(camPosition, camTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Camera::Reset() noexcept
{
  m_position = {0.0f,0.0f,-3.0f};
  m_pitch = 0.0f;
  m_yaw = 0.0f;
}
