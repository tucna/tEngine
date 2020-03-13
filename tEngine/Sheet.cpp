#include "IndexBuffer.h"
#include "InputLayout.h"
#include "Plane.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "Sheet.h"
#include "Topology.h"
#include "TransformConstantBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

Sheet::Sheet(Graphics& gfx) :
  m_vx(0.0f),
  m_vy(0.0f),
  m_vz(0.0f),
  m_pitch(0.0f),
  m_roll(0.0f),
  m_yaw(0.0f)
{
  if (!IsStaticInitialized())
  {
    struct Vertex
    {
      DirectX::XMFLOAT3 pos;
      struct
      {
        float u;
        float v;
      } tex;
    };

    auto model = Plane::Make<Vertex>();
    model.m_vertices[0].tex = { 0.0f,0.0f };
    model.m_vertices[1].tex = { 1.0f,0.0f };
    model.m_vertices[2].tex = { 0.0f,1.0f };
    model.m_vertices[3].tex = { 1.0f,1.0f };

    //AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\kappa50.png"))); TUCNA

    AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.m_vertices));

    AddStaticBind(std::make_unique<Sampler>(gfx));

    auto pvs = std::make_unique<VertexShader>(gfx, L"Texture_VS.cso");
    auto pvsbc = pvs->GetBytecode();
    AddStaticBind(std::move(pvs));

    AddStaticBind(std::make_unique<PixelShader>(gfx, L"Texture_PS.cso"));

    AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_indices));

    const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
    {
      { "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
      { "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
    };
    AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

    AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
  }
  else
  {
    SetIndexFromStatic();
  }

  AddBind(std::make_unique<TransformConstantBuffer>(gfx, *this));
}

void Sheet::Update(float dt) noexcept
{
  m_roll += 0.5f * dt;
  m_pitch += 0.5f * dt;
  m_yaw += 0.5f * dt;
}

DirectX::XMMATRIX Sheet::GetTransformXM() const noexcept
{
  return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
    DirectX::XMMatrixTranslation(m_vx, m_vy, m_vz);
}
