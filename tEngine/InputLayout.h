#pragma once

#include "Common.h"

#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{

class InputLayout : public Bindable
{
public:
  InputLayout(Graphics& gfx, Dvtx::VertexLayout layout, ID3DBlob* pVertexShaderBytecode);

  void Bind(Graphics& gfx) noexcept override;
  std::string GetUID() const noexcept override;

  static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode);
  static std::string GenerateUID(const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode = nullptr);

private:
  Dvtx::VertexLayout m_layout;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};

}
