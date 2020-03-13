#pragma once

#include "Common.h"

#include "ConstantBuffers.h"
#include "Graphics.h"

class PointLight
{
public:
  PointLight(Graphics& gfx);

  void Reset() noexcept;
  void Bind(Graphics& gfx) const noexcept;

private:
  struct PointLightCBuf
  {
    alignas(16) DirectX::XMFLOAT3 pos;
    alignas(16) DirectX::XMFLOAT3 ambient;
    alignas(16) DirectX::XMFLOAT3 diffuseColor;

    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
  };

  PointLightCBuf m_cbData;
  mutable PixelConstantBuffer<PointLightCBuf> m_cbuf;

};

