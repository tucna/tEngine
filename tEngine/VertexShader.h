#pragma once

#include "Common.h"

#include <memory>
#include <string>

#include "Bindable.h"

namespace Bind
{

class VertexShader : public Bindable
{
public:
  VertexShader(Graphics& gfx, const std::string& path);

  void Bind(Graphics& gfx) noexcept override;
  std::string GetUID() const noexcept override;

  static std::shared_ptr<VertexShader> Resolve(Graphics& gfx, const std::string& path);
  static std::string GenerateUID(const std::string& path);

  ID3DBlob* GetBytecode() const noexcept { return m_blob.Get(); }

private:
  std::string m_path;

  Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
};

}
