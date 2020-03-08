#pragma once

#include "Common.h"

#include <string>

#include "Bindable.h"

class VertexShader : public Bindable
{
public:
  VertexShader(Graphics& gfx, const std::wstring& path);

  void Bind(Graphics& gfx) noexcept override;

  ID3DBlob* GetBytecode() const noexcept { return m_blob.Get(); }

private:
  Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
};
