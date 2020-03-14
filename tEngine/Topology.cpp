#include "Topology.h"

using namespace Bind;

Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type) :
  m_type(type)
{
}

void Topology::Bind(Graphics& gfx) noexcept
{
  GetContext(gfx)->IASetPrimitiveTopology(m_type);
}
