#include "PointLight.h"

PointLight::PointLight(Graphics& gfx) :
  m_cbuf(gfx)
{
  Reset();
}

void PointLight::Reset() noexcept
{
  m_cbData = {
    { 0.0f,-9.0f,0.0f },
    { 0.15f,0.15f,0.15f },
    { 1.0f,1.0f,1.0f },
    1.0f,
    1.0f,
    0.045f,
    0.0075f,
  };
}

void PointLight::Bind(Graphics& gfx) const noexcept
{
  m_cbuf.Update(gfx, m_cbData);
  m_cbuf.Bind(gfx);
}
