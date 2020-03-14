#include "VertexShader.h"

using namespace Bind;

VertexShader::VertexShader(Graphics& gfx, const std::wstring& path)
{
  CHECK_HR(D3DReadFileToBlob(path.c_str(), &m_blob));
  CHECK_HR(GetDevice(gfx)->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_vertexShader));
}

void VertexShader::Bind(Graphics & gfx) noexcept
{
  GetContext(gfx)->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
}
