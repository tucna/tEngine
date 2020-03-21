#pragma once

#include "Common.h"

#include <vector>

#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{

class VertexBuffer : public Bindable
{
public:
  VertexBuffer(Graphics& gfx, const std::string& tag, const Dvtx::VertexBuffer& vbuf);
  VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf);

  void Bind(Graphics& gfx) noexcept override;
  std::string GetUID() const noexcept override;

  static std::shared_ptr<VertexBuffer> Resolve(Graphics& gfx, const std::string& tag, const Dvtx::VertexBuffer& vbuf);

  template<typename...Ignore>
  static std::string GenerateUID(const std::string& tag, Ignore&&...ignore) { return GenerateUID_(tag); }

private:
  static std::string GenerateUID_(const std::string& tag);

  std::string m_tag;
  UINT m_stride;

  Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
};

}