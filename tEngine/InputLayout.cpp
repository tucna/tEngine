#include "InputLayout.h"

InputLayout::InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* vertexShaderBytecode)
{
  CHECK_HR(GetDevice(gfx)->CreateInputLayout(
    layout.data(), (UINT)layout.size(),
    vertexShaderBytecode->GetBufferPointer(),
    vertexShaderBytecode->GetBufferSize(),
    &m_inputLayout
  ));
}

void InputLayout::Bind(Graphics & gfx) noexcept
{
  GetContext(gfx)->IASetInputLayout(m_inputLayout.Get());
}
