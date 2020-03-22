#include "PointLight.h"

PointLight::PointLight(Graphics& gfx, float radius) :
  m_mesh(gfx, radius),
  m_cbuf(gfx)
{
  Reset();
}

void PointLight::Reset() noexcept
{
  m_cbData = {
    { 10.0f,9.0f,2.5f },
    { 0.05f,0.05f,0.05f },
    { 1.0f,1.0f,1.0f },
    1.0f,
    1.0f,
    0.045f,
    0.0075f,
  };
}

void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{
  auto dataCopy = m_cbData;
  const auto pos = DirectX::XMLoadFloat3(&m_cbData.pos);

  DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));

  m_cbuf.Update(gfx, dataCopy);
  m_cbuf.Bind(gfx);
}

void PointLight::Translate(DirectX::XMFLOAT3 translation) noexcept
{
  m_cbData.pos.x += translation.x;
  m_cbData.pos.y += translation.y;
  m_cbData.pos.z += translation.z;
}

void PointLight::Draw(Graphics& gfx) const noexcept
{
  m_mesh.SetPos(m_cbData.pos);
  m_mesh.Draw(gfx);
}
