#pragma once

#include <string>

#include "Bindable.h"

class PixelShader : public Bindable
{
public:
  PixelShader(Graphics& gfx, const std::wstring& file);

  void Bind(Graphics& gfx) noexcept override;

protected:
  Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
};

