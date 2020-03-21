#pragma once

#include "Common.h"

#include <vector>

#include "Bindable.h"

namespace Bind
{

class IndexBuffer : public Bindable
{
public:
  IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
  IndexBuffer(Graphics& gfx, std::string tag, const std::vector<unsigned short>& indices);

  void Bind(Graphics& gfx) noexcept override;
  std::string GetUID() const noexcept override;

  UINT GetCount() const noexcept { return m_count; }

  static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, const std::string& tag, const std::vector<unsigned short>& indices);

  template<typename...Ignore>
  static std::string GenerateUID(const std::string& tag, Ignore&&...ignore) { return GenerateUID_(tag); }

private:
  static std::string GenerateUID_(const std::string& tag);

  std::string m_tag;
  UINT m_count;

  Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};

}
