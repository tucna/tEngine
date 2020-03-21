#include "Codex.h"
#include "VertexShader.h"

Bind::VertexShader::VertexShader(Graphics& gfx, const std::string& path) :
  m_path(path)
{
  CHECK_HR(D3DReadFileToBlob(Utils::To_wstring(path).c_str(), &m_blob));
  CHECK_HR(GetDevice(gfx)->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_vertexShader));
}

void Bind::VertexShader::Bind(Graphics & gfx) noexcept
{
  GetContext(gfx)->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
}

std::string Bind::VertexShader::GetUID() const noexcept
{
  return GenerateUID(m_path);
}

std::shared_ptr<Bind::VertexShader> Bind::VertexShader::Resolve(Graphics & gfx, const std::string& path)
{
  return Codex::Resolve<VertexShader>(gfx, path);
}

std::string Bind::VertexShader::GenerateUID(const std::string & path)
{
  using namespace std::string_literals;

  return typeid(Bind::VertexShader).name() + "#"s + path;
}
