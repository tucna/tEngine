#include <unordered_map>
#include <utility>
#include <vector>

#include "IndexBuffer.h"
#include "InputLayout.h"
#include "Mesh.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformConstantBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

using namespace DirectX;

// Mesh
Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs)
{
  if (!IsStaticInitialized())
  {
    AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
  }

  for (auto& pb : bindPtrs)
  {
    if (auto pi = dynamic_cast<Bind::IndexBuffer*>(pb.get()))
    {
      AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer>{ pi });
      pb.release();
    }
    else
    {
      AddBind(std::move(pb));
    }
  }

  AddBind(std::make_unique<Bind::TransformConstantBuffer>(gfx, *this));
}
void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
  DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
  Drawable::Draw(gfx);
}
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
  return DirectX::XMLoadFloat4x4(&transform);
}


// Node
Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noexcept
  :
  id(id),
  meshPtrs(std::move(meshPtrs)),
  name(name)
{
  XMStoreFloat4x4(&transform, transform_in);
  XMStoreFloat4x4(&appliedTransform, XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
  const auto built =
    XMLoadFloat4x4(&appliedTransform) *
    XMLoadFloat4x4(&transform) *
    accumulatedTransform;
  for (const auto pm : meshPtrs)
  {
    pm->Draw(gfx, built);
  }
  for (const auto& pc : childPtrs)
  {
    pc->Draw(gfx, built);
  }
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept
{
  assert(pChild);
  childPtrs.push_back(std::move(pChild));
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
  XMStoreFloat4x4(&appliedTransform, transform);
}

int Node::GetId() const noexcept
{
  return id;
}


// Model
class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:
  XMMATRIX GetTransform() const noexcept
  {
    assert(pSelectedNode != nullptr);
    const auto& transform = transforms.at(pSelectedNode->GetId());
    return
      XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
      XMMatrixTranslation(transform.x, transform.y, transform.z);
  }
  Node* GetSelectedNode() const noexcept
  {
    return pSelectedNode;
  }
private:
  Node* pSelectedNode;
  struct TransformParameters
  {
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
  };
  std::unordered_map<int, TransformParameters> transforms;
};

Model::Model(Graphics& gfx, const std::string fileName)
  :
  pWindow(std::make_unique<ModelWindow>())
{
  Assimp::Importer imp;
  const auto pScene = imp.ReadFile(fileName.c_str(),
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |
    aiProcess_ConvertToLeftHanded |
    aiProcess_GenNormals
  );

  for (size_t i = 0; i < pScene->mNumMeshes; i++)
    meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));

  int nextId = 0;
  pRoot = ParseNode(nextId, *pScene->mRootNode);
}

void Model::Draw(Graphics& gfx) const noexcept
{
  if (auto node = pWindow->GetSelectedNode())
  {
    node->SetAppliedTransform(pWindow->GetTransform());
  }
  pRoot->Draw(gfx, XMMatrixIdentity());
}

Model::~Model() noexcept
{}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh)
{
  using Dvtx::VertexLayout;

  Dvtx::VertexBuffer vbuf(std::move(
    VertexLayout{}
    .Append(VertexLayout::Position3D)
    .Append(VertexLayout::Normal)
  ));

  for (unsigned int i = 0; i < mesh.mNumVertices; i++)
  {
    vbuf.EmplaceBack(
      *reinterpret_cast<XMFLOAT3*>(&mesh.mVertices[i]),
      *reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i])
    );
  }

  std::vector<unsigned short> indices;
  indices.reserve(mesh.mNumFaces * 3);
  for (unsigned int i = 0; i < mesh.mNumFaces; i++)
  {
    const auto& face = mesh.mFaces[i];
    assert(face.mNumIndices == 3);
    indices.push_back(face.mIndices[0]);
    indices.push_back(face.mIndices[1]);
    indices.push_back(face.mIndices[2]);
  }

  std::vector<std::unique_ptr<Bind::Bindable>> bindablePtrs;

  bindablePtrs.push_back(std::make_unique<Bind::VertexBuffer>(gfx, vbuf));

  bindablePtrs.push_back(std::make_unique<Bind::IndexBuffer>(gfx, indices));

  auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"PhongVS.cso");
  auto pvsbc = pvs->GetBytecode();
  bindablePtrs.push_back(std::move(pvs));

  bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso"));

  bindablePtrs.push_back(std::make_unique<Bind::InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

  struct PSMaterialConstant
  {
    DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
    float specularIntensity = 0.6f;
    float specularPower = 30.0f;
    float padding[3];
  } pmc;
  bindablePtrs.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

  return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
{
  namespace dx = DirectX;
  const auto transform = XMMatrixTranspose(XMLoadFloat4x4(
    reinterpret_cast<const XMFLOAT4X4*>(&node.mTransformation)
  ));

  std::vector<Mesh*> curMeshPtrs;
  curMeshPtrs.reserve(node.mNumMeshes);
  for (size_t i = 0; i < node.mNumMeshes; i++)
  {
    const auto meshIdx = node.mMeshes[i];
    curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
  }

  auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
  for (size_t i = 0; i < node.mNumChildren; i++)
  {
    pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
  }

  return pNode;
}
