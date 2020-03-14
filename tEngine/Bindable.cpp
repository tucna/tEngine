#include "Bindable.h"

using namespace Bind;

ID3D11DeviceContext* Bindable::GetContext(Graphics& gfx) noexcept
{
  return gfx.m_context.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics& gfx) noexcept
{
  return gfx.m_device.Get();
}
