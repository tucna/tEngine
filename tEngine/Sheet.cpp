#include "IndexBuffer.h"
#include "InputLayout.h"
#include "Plane.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "Sheet.h"
#include "Topology.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

Sheet::Sheet(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist) :
  r(rdist(rng)),
  droll(ddist(rng)),
  dpitch(ddist(rng)),
  dyaw(ddist(rng)),
  dphi(odist(rng)),
  dtheta(odist(rng)),
  dchi(odist(rng)),
  chi(adist(rng)),
  theta(adist(rng)),
  phi(adist(rng))
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

    auto pvs = std::make_unique<VertexShader>(gfx, L"TextureVS.cso");
    auto pvsbc = pvs->GetBytecode();
    AddStaticBind(std::move(pvs));

    AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturePS.cso"));

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

  //AddBind(std::make_unique<TransformCbuf>(gfx, *this)); TUCNA
}

void Sheet::Update(float dt) noexcept
{
  roll += droll * dt;
  pitch += dpitch * dt;
  yaw += dyaw * dt;
  theta += dtheta * dt;
  phi += dphi * dt;
  chi += dchi * dt;
}

DirectX::XMMATRIX Sheet::GetTransformXM() const noexcept
{
  return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
    DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
    DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
