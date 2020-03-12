#include "Box.h"
#include "Cube.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformConstantBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

using namespace DirectX;

Box::Box(Graphics & gfx)
{
  if (!IsStaticInitialized())
  {
    struct Vertex
    {
      XMFLOAT3 pos;
      XMFLOAT3 n;
    };

    auto model = Cube::MakeIndependent<Vertex>();
    //model.SetNormalsIndependentFlat(); TUCNA

    AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.m_vertices));

    auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
    auto pvsbc = pvs->GetBytecode();
    AddStaticBind(std::move(pvs));

    AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

    AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_indices));

    const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
    {
      { "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
      { "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
    };
    AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

    AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
  }
  else
  {
    SetIndexFromStatic();
  }

  AddBind(std::make_unique<TransformConstantBuffer>(gfx, *this));

  // model deformation transform (per instance, not stored as bind)
  XMStoreFloat3x3(
    &m_model,
    XMMatrixScaling(1.0f, 1.0f, 1.0f)
  );
}

void Box::Update(float dt) noexcept
{
  roll += 0.5f * dt;
  pitch += 0.5f * dt;
  yaw += 0.5f * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
  return DirectX::XMLoadFloat3x3(&m_model) *
    DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
    DirectX::XMMatrixTranslation(r, 0.0f, 0.0f);
}
