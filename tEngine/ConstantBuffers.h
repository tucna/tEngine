#pragma once

#include "Common.h"

#include "Bindable.h"
#include "Codex.h"
#include "Graphics.h"

namespace Bind
{

template<typename C>
class ConstantBuffer : public Bindable
{
public:
  ConstantBuffer(Graphics& gfx, UINT slot = 0u) :
    m_slot(slot)
  {
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(C);
    cbd.StructureByteStride = 0u;

    CHECK_HR(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &m_constantBuffer));
  }

  ConstantBuffer(Graphics& gfx, const C& consts, UINT slot = 0u) :
    m_slot(slot)
  {
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(consts);
    cbd.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &consts;

    CHECK_HR(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &m_constantBuffer));
  }

  void Update(Graphics& gfx, const C& consts)
  {
    D3D11_MAPPED_SUBRESOURCE msr;

    CHECK_HR(GetContext(gfx)->Map(m_constantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
    memcpy(msr.pData, &consts, sizeof(consts));
    GetContext(gfx)->Unmap(m_constantBuffer.Get(), 0u);
  }

protected:
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

  UINT m_slot;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
  using ConstantBuffer<C>::m_constantBuffer;
  using ConstantBuffer<C>::m_slot;
  using Bindable::GetContext;

public:
  using ConstantBuffer<C>::ConstantBuffer;

  void Bind(Graphics& gfx) noexcept override
  {
    GetContext(gfx)->VSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
  }

  static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, const C& consts, UINT slot = 0)
  {
    return Codex::Resolve<VertexConstantBuffer>(gfx, consts, slot);
  }

  static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, UINT slot = 0)
  {
    return Codex::Resolve<VertexConstantBuffer>(gfx, slot);
  }

  static std::string GenerateUID(const C&, UINT slot)
  {
    return GenerateUID(slot);
  }

  static std::string GenerateUID(UINT slot = 0)
  {
    using namespace std::string_literals;

    return typeid(VertexConstantBuffer).name() + "#"s + std::to_string(slot);
  }

  std::string GetUID() const noexcept override
  {
    return GenerateUID(m_slot);
  }
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
  using ConstantBuffer<C>::m_constantBuffer;
  using ConstantBuffer<C>::m_slot;
  using Bindable::GetContext;

public:
  using ConstantBuffer<C>::ConstantBuffer;

  void Bind(Graphics& gfx) noexcept override
  {
    GetContext(gfx)->PSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
  }

  static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, const C& consts, UINT slot = 0)
  {
    return Codex::Resolve<PixelConstantBuffer>(gfx, consts, slot);
  }

  static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, UINT slot = 0)
  {
    return Codex::Resolve<PixelConstantBuffer>(gfx, slot);
  }

  static std::string GenerateUID(const C&, UINT slot)
  {
    return GenerateUID(slot);
  }

  static std::string GenerateUID(UINT slot = 0)
  {
    using namespace std::string_literals;
    return typeid(PixelConstantBuffer).name() + "#"s + std::to_string(slot);
  }

  std::string GetUID() const noexcept override
  {
    return GenerateUID(m_slot);
  }
};

}
