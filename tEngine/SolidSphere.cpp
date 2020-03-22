#include "ConstantBuffers.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "SolidSphere.h"
#include "Sphere.h"
#include "Topology.h"
#include "TransformConstantBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

using namespace DirectX;

SolidSphere::SolidSphere(Graphics & gfx, float radius) :
  m_position(1.0f,1.0f,1.0f)
{
  auto model = Sphere::Make();
  model.Transform(XMMatrixScaling(radius, radius, radius));
  const auto geometryTag = "$sphere." + std::to_string(radius);
  AddBind(Bind::VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
  AddBind(Bind::IndexBuffer::Resolve(gfx, geometryTag, model.indices));

  auto pvs = Bind::VertexShader::Resolve(gfx, "SolidVS.cso");
  auto pvsbc = pvs->GetBytecode();
  AddBind(std::move(pvs));

  AddBind(Bind::PixelShader::Resolve(gfx, "SolidPS.cso"));

  struct PSColorConstant
  {
    XMFLOAT3 color = {1.0f,1.0f,1.0f};
    float padding;
  } colorConst;

  AddBind(Bind::PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));

  AddBind(Bind::InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

  AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

  AddBind(std::make_shared<Bind::TransformConstantBuffer>(gfx, *this));
}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
  m_position = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
  return DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}
