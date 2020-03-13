#include "Box.h"
#include "Cube.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformConstantBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

using namespace DirectX;

Box::Box(Graphics& gfx, DirectX::XMFLOAT3 material) :
  m_vx(0.0f),
  m_vy(0.0f),
  m_vz(2.0f),
  m_pitch(0.0f),
  m_roll(0.0f),
  m_yaw(0.0f)
{
  if (!IsStaticInitialized())
  {
    struct Vertex
    {
      XMFLOAT3 pos;
      XMFLOAT3 n;
    };

    auto model = Cube::MakeIndependent<Vertex>();
    model.SetNormalsIndependentFlat();

    AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.m_vertices));

    auto pvs = std::make_unique<VertexShader>(gfx, L"Phong_VS.cso");
    auto pvsbc = pvs->GetBytecode();
    AddStaticBind(std::move(pvs));

    AddStaticBind(std::make_unique<PixelShader>(gfx, L"Phong_PS.cso"));

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

  struct PSMaterialConstant
  {
    XMFLOAT3 color;
    float padding;
  } colorConst;

  colorConst.color = material;
  AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));


  XMStoreFloat3x3(&m_model, XMMatrixScaling(1.0f, 1.0f, 1.0f));
}

void Box::Update(float dt) noexcept
{
  m_roll += 0.5f * dt;
  m_pitch += 0.5f * dt;
  m_yaw += 0.5f * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
  return DirectX::XMLoadFloat3x3(&m_model) *
    DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
    DirectX::XMMatrixTranslation(m_vx, m_vy, m_vz);
}
