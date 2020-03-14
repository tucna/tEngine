#include "Common.h"

#include "PixelShader.h"

using namespace Bind;

PixelShader::PixelShader(Graphics& gfx, const std::wstring& file)
{
  Microsoft::WRL::ComPtr<ID3DBlob> blob;

  CHECK_HR(D3DReadFileToBlob(file.c_str(), &blob));
  CHECK_HR(GetDevice(gfx)->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_pixelShader));
}

void PixelShader::Bind(Graphics& gfx) noexcept
{
  GetContext(gfx)->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
}
