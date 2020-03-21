#include "Codex.h"
#include "InputLayout.h"

Bind::InputLayout::InputLayout(Graphics& gfx, Dvtx::VertexLayout layout, ID3DBlob* pVertexShaderBytecode) :
  m_layout(std::move(layout))
{
  const auto d3dLayout = m_layout.GetD3DLayout();

  CHECK_HR(GetDevice(gfx)->CreateInputLayout(
    d3dLayout.data(), (UINT)d3dLayout.size(),
    pVertexShaderBytecode->GetBufferPointer(),
    pVertexShaderBytecode->GetBufferSize(),
    &m_inputLayout
  ));
}

void Bind::InputLayout::Bind(Graphics & gfx) noexcept
{
  GetContext(gfx)->IASetInputLayout(m_inputLayout.Get());
}

std::string Bind::InputLayout::GetUID() const noexcept
{
  return GenerateUID(m_layout);
}

std::shared_ptr<Bind::InputLayout> Bind::InputLayout::Resolve(Graphics& gfx, const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode)
{
  return Codex::Resolve<Bind::InputLayout>(gfx, layout, pVertexShaderBytecode);
}

std::string Bind::InputLayout::GenerateUID(const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode)
{
  using namespace std::string_literals;

  return typeid(Bind::InputLayout).name() + "#"s + layout.GetCode();
}
