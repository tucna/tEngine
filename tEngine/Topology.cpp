#include "Codex.h"
#include "Topology.h"

Bind::Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type) :
  m_type(type)
{
}

void Bind::Topology::Bind(Graphics& gfx) noexcept
{
  GetContext(gfx)->IASetPrimitiveTopology(m_type);
}

std::shared_ptr<Bind::Topology> Bind::Topology::Resolve(Graphics & gfx, D3D11_PRIMITIVE_TOPOLOGY type)
{
  return Codex::Resolve<Bind::Topology>(gfx, type);
}

std::string Bind::Topology::GenerateUID(D3D11_PRIMITIVE_TOPOLOGY type)
{
  using namespace std::string_literals;

  return typeid(Bind::Topology).name() + "#"s + std::to_string(type);
}

std::string Bind::Topology::GetUID() const noexcept
{
  return GenerateUID(m_type);
}
