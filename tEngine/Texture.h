#pragma once

#include "Common.h"

#include <string>
#include <string_view>

#include "Bindable.h"

namespace Bind
{

class Texture : public Bindable
{
public:
  Texture(Graphics& gfx, std::string_view path, UINT slot = 0);

  void Bind(Graphics& gfx) noexcept override;

  std::string GetUID() const noexcept override;

  static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0);
  static std::string GenerateUID(const std::string& path, UINT slot = 0);

  bool HasAlpha() const noexcept { return m_hasAlpha; }

private:
  uint8_t m_slot;
  bool m_hasAlpha;
  std::string m_path;

  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
};

}
