#include "Codex.h"
#include "Texture.h"
#include "WICTextureLoader.h"

using namespace DirectX;

Bind::Texture::Texture(Graphics& gfx, std::string_view path, bool mipmaps, UINT slot) :
  m_path(path),
  m_slot(slot),
  m_hasAlpha(true) // TODO: fix this
{
  if (mipmaps)
  {
    CHECK_HR(CreateWICTextureFromFile(GetDevice(gfx), GetContext(gfx), Utils::To_wstring(path.data()).c_str(), nullptr, &m_textureView));
  }
  else
  {
    CHECK_HR(CreateWICTextureFromFile(GetDevice(gfx), nullptr, Utils::To_wstring(path.data()).c_str(), nullptr, &m_textureView));
  }
}

void Bind::Texture::Bind(Graphics& gfx) noexcept
{
  GetContext(gfx)->PSSetShaderResources(m_slot, 1u, m_textureView.GetAddressOf());
}

std::string Bind::Texture::GetUID() const noexcept
{
  return GenerateUID(m_path, m_slot); // TODO: not necessary to generate again and again
}

std::shared_ptr<Bind::Texture> Bind::Texture::Resolve(Graphics & gfx, const std::string& path, UINT slot)
{
  return Codex::Resolve<Texture>(gfx, path, slot);
}

std::string Bind::Texture::GenerateUID(const std::string& path, UINT slot)
{
  using namespace std::string_literals;

  return typeid(Bind::Texture).name() + "#"s + path + "#" + std::to_string(slot);
}
