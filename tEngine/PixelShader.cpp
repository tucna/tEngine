#include "Codex.h"
#include "PixelShader.h"

Bind::PixelShader::PixelShader(Graphics& gfx, const std::string& path) : // TODO: use wstring
  m_path(path)
{
  Microsoft::WRL::ComPtr<ID3DBlob> blob;

  CHECK_HR(D3DReadFileToBlob(std::wstring{ path.begin(),path.end() }.c_str(), &blob));
  CHECK_HR(GetDevice(gfx)->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_pixelShader));
}

void Bind::PixelShader::Bind(Graphics& gfx) noexcept
{
  GetContext(gfx)->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
}

std::shared_ptr<Bind::PixelShader> Bind::PixelShader::Resolve(Graphics& gfx, const std::string& path)
{
  return Codex::Resolve<Bind::PixelShader>(gfx, path);
}

std::string Bind::PixelShader::GenerateUID(const std::string& path)
{
  using namespace std::string_literals;

  return typeid(PixelShader).name() + "#"s + path;
}

std::string Bind::PixelShader::GetUID() const noexcept
{
  return GenerateUID(m_path);
}
