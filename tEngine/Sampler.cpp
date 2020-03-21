#include "Codex.h"
#include "Sampler.h"

Bind::Sampler::Sampler(Graphics& gfx)
{
  D3D11_SAMPLER_DESC samplerDesc = {};
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

  CHECK_HR(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &m_sampler));
}

void Bind::Sampler::Bind(Graphics & gfx) noexcept
{
  GetContext(gfx)->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
}

std::shared_ptr<Bind::Sampler> Bind::Sampler::Resolve(Graphics & gfx)
{
  return Codex::Resolve<Sampler>(gfx);
}

std::string Bind::Sampler::GenerateUID()
{
  return typeid(Sampler).name();
}
