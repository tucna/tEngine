#pragma once

#include "Bindable.h"

namespace Bind
{

class Sampler : public Bindable
{
public:
  Sampler(Graphics& gfx);

  void Bind(Graphics& gfx) noexcept override;

  static std::shared_ptr<Sampler> Resolve(Graphics& gfx);
  static std::string GenerateUID();

  std::string GetUID() const noexcept override { return GenerateUID(); }

private:
  Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
};

}
