#include <algorithm>

#include "Camera.h"

using namespace DirectX;

// TUCNA utility
template<typename T>
T wrap_angle(T theta)
{
  const T modded = fmod(theta, (T)2.0 * (T)PI_D);
  return (modded > (T)PI_D) ?
    (modded - (T)2.0 * (T)PI_D) :
    modded;
}
// -----------------

Camera::Camera() noexcept
{
  Reset();
}

void Camera::Rotate(float dx, float dy) noexcept
{
  m_yaw = wrap_angle(m_yaw + dx * rotationSpeed);
  m_pitch = std::clamp(m_pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
  XMStoreFloat3(&translation, XMVector3Transform(
    XMLoadFloat3(&translation),
    XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f) *
    XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
  ));
  m_position = {
    m_position.x + translation.x,
    m_position.y + translation.y,
    m_position.z + translation.z
  };
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
