#pragma once

#include "Graphics.h"

#include <string>

namespace Bind
{

class Bindable
{
public:
  virtual void Bind(Graphics& gfx) noexcept = 0;

  virtual ~Bindable() = default;

  virtual std::string GetUID() const noexcept = 0;

protected:
  static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept { return gfx.m_context.Get(); }
  static ID3D11Device* GetDevice(Graphics& gfx) noexcept { return gfx.m_device.Get(); }
};

}
