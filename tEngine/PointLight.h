#pragma once

#include "Common.h"

#include "ConstantBuffers.h"
#include "Graphics.h"
#include "SolidSphere.h"

class PointLight
{
public:
  PointLight(Graphics& gfx, float radius = 0.5f);

  void Reset() noexcept;

  void Draw(Graphics& gfx) const noexcept;
  void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;

  void Translate(DirectX::XMFLOAT3 translation) noexcept;

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

  mutable SolidSphere m_mesh;
  mutable Bind::PixelConstantBuffer<PointLightCBuf> m_cbuf;
};

