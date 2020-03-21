#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ConstantBuffers.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "Mesh.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "Texture.h"
#include "Topology.h"
#include "TransformConstantBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

using namespace DirectX;

class ModelWindow
{
public:
  XMMATRIX GetTransform() const noexcept
  {
    assert(m_selectedNode != nullptr);

    const auto& transform = m_transforms.at(m_selectedNode->GetId());

    return
      XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
      XMMatrixTranslation(transform.x, transform.y, transform.z);
  }

  Node* GetSelectedNode() const noexcept { return m_selectedNode; }

private:
  struct TransformParameters
  {
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
  };

  Node* m_selectedNode;

  std::unordered_map<int, TransformParameters> m_transforms;
};

Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs)
{
  AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

  for (auto& pb : bindPtrs)
    AddBind(std::move(pb));

  AddBind(std::make_shared<Bind::TransformConstantBuffer>(gfx, *this));
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
  DirectX::XMStoreFloat4x4(&m_transform, accumulatedTransform);
  Drawable::Draw(gfx);
}

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept :
  m_id(id),
  m_meshPtrs(std::move(meshPtrs)),
  m_name(name)
{
  XMStoreFloat4x4(&m_transform, transform);
  XMStoreFloat4x4(&m_appliedTransform, XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
  const auto built =
    XMLoadFloat4x4(&m_appliedTransform) *
    XMLoadFloat4x4(&m_transform) *
    accumulatedTransform;

  for (const auto pm : m_meshPtrs)
    pm->Draw(gfx, built);

  for (const auto& pc : m_childPtrs)
    pc->Draw(gfx, built);
}

void Node::AddChild(std::unique_ptr<Node> child) noexcept
{
  assert(child);

  m_childPtrs.push_back(std::move(child));
}

Model::Model(Graphics& gfx, std::string_view fileName, float scale) :
  m_window(std::make_unique<ModelWindow>())
{
  Assimp::Importer imp;
  const auto pScene = imp.ReadFile(fileName.data(),
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |
    aiProcess_ConvertToLeftHanded |
    aiProcess_GenNormals |
    aiProcess_CalcTangentSpace
  );

  for (size_t i = 0; i < pScene->mNumMeshes; i++)
    m_meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, fileName, scale));

  int nextId = 0;
  m_root = ParseNode(nextId, *pScene->mRootNode);
}

Model::~Model()
{
}

void Model::Draw(Graphics& gfx) const noexcept
{
  if (auto node = m_window->GetSelectedNode())
    node->SetAppliedTransform(m_window->GetTransform());

  m_root->Draw(gfx, XMMatrixIdentity());
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale)
{
  const auto rootPath = path.parent_path().string() + "\\";

  std::vector<std::shared_ptr<Bind::Bindable>> bindablePtrs;

  bool hasSpecularMap = false;
  bool hasAlphaGloss = false;
  bool hasNormalMap = false;
  bool hasDiffuseMap = false;

  float shininess = 2.0f;

  XMFLOAT4 specularColor = { 0.18f,0.18f,0.18f,1.0f };
  XMFLOAT4 diffuseColor = { 0.45f,0.45f,0.85f,1.0f };

  if (mesh.mMaterialIndex >= 0)
  {
    auto& material = *pMaterials[mesh.mMaterialIndex];

    aiString texFileName;

    if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
    {
      bindablePtrs.push_back(Bind::Texture::Resolve(gfx, rootPath + texFileName.C_Str()));
      hasDiffuseMap = true;
    }
    else
    {
      material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));
    }

    if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
    {
      auto tex = Bind::Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
      hasAlphaGloss = tex->HasAlpha();
      bindablePtrs.push_back(std::move(tex));
      hasSpecularMap = true;
    }
    else
    {
      material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));
    }

    if (!hasAlphaGloss)
      material.Get(AI_MATKEY_SHININESS, shininess);

    if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
    {
      auto tex = Bind::Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2);
      hasAlphaGloss = tex->HasAlpha();
      bindablePtrs.push_back(std::move(tex));
      hasNormalMap = true;
    }

    if (hasDiffuseMap || hasSpecularMap || hasNormalMap)
      bindablePtrs.push_back(Bind::Sampler::Resolve(gfx));
  }

  const auto meshTag = path.string() + "%" + mesh.mName.C_Str();

  if (hasDiffuseMap && hasNormalMap && hasSpecularMap)
  {
    Dvtx::VertexBuffer vbuf(std::move(Dvtx::VertexLayout{}
      .Append(Dvtx::VertexLayout::Position3D)
      .Append(Dvtx::VertexLayout::Normal)
      .Append(Dvtx::VertexLayout::Tangent)
      .Append(Dvtx::VertexLayout::Bitangent)
      .Append(Dvtx::VertexLayout::Texture2D)
    ));

    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
      vbuf.EmplaceBack(
        XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
        *reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
        *reinterpret_cast<XMFLOAT3*>(&mesh.mTangents[i]),
        *reinterpret_cast<XMFLOAT3*>(&mesh.mBitangents[i]),
        *reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
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

    bindablePtrs.push_back(Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf));

    bindablePtrs.push_back(Bind::IndexBuffer::Resolve(gfx, meshTag, indices));

    auto pvs = Bind::VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
    auto pvsbc = pvs->GetBytecode();
    bindablePtrs.push_back(std::move(pvs));

    bindablePtrs.push_back(Bind::PixelShader::Resolve(gfx, "PhongPSSpecNormalMap.cso"));

    bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

    Node::PSMaterialConstantFullmonte pmc;
    pmc.specularPower = shininess;
    pmc.hasGlossMap = hasAlphaGloss ? TRUE : FALSE;

    // TODO: this is CLEARLY an issue... all meshes will share same mat const, but may have different
    // Ns (specular power) specified for each in the material properties... bad conflict
    bindablePtrs.push_back(Bind::PixelConstantBuffer<Node::PSMaterialConstantFullmonte>::Resolve(gfx, pmc, 1u));
  }
  else if (hasDiffuseMap && hasNormalMap)
  {
    Dvtx::VertexBuffer vbuf(std::move(Dvtx::VertexLayout{}
      .Append(Dvtx::VertexLayout::Position3D)
      .Append(Dvtx::VertexLayout::Normal)
      .Append(Dvtx::VertexLayout::Tangent)
      .Append(Dvtx::VertexLayout::Bitangent)
      .Append(Dvtx::VertexLayout::Texture2D)
    ));

    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
      vbuf.EmplaceBack(
        XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
        *reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
        *reinterpret_cast<XMFLOAT3*>(&mesh.mTangents[i]),
        *reinterpret_cast<XMFLOAT3*>(&mesh.mBitangents[i]),
        *reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
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

    bindablePtrs.push_back(Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf));

    bindablePtrs.push_back(Bind::IndexBuffer::Resolve(gfx, meshTag, indices));

    auto pvs = Bind::VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
    auto pvsbc = pvs->GetBytecode();
    bindablePtrs.push_back(std::move(pvs));

    bindablePtrs.push_back(Bind::PixelShader::Resolve(gfx, "PhongPSNormalMap.cso"));

    bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

    struct PSMaterialConstantDiffnorm
    {
      float specularIntensity;
      float specularPower;
      BOOL  normalMapEnabled = TRUE;
      float padding[1];
    } pmc;

    pmc.specularPower = shininess;
    pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;

    // this is CLEARLY an issue... all meshes will share same mat const, but may have different
    // Ns (specular power) specified for each in the material properties... bad conflict
    bindablePtrs.push_back(Bind::PixelConstantBuffer<PSMaterialConstantDiffnorm>::Resolve(gfx, pmc, 1u));
  }
  else if (hasDiffuseMap && !hasNormalMap && hasSpecularMap)
  {
    Dvtx::VertexBuffer vbuf(std::move(Dvtx::VertexLayout{}
      .Append(Dvtx::VertexLayout::Position3D)
      .Append(Dvtx::VertexLayout::Normal)
      .Append(Dvtx::VertexLayout::Texture2D)
    ));

    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
      vbuf.EmplaceBack(
        XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
        *reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
        *reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
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

    bindablePtrs.push_back(Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf));

    bindablePtrs.push_back(Bind::IndexBuffer::Resolve(gfx, meshTag, indices));

    auto pvs = Bind::VertexShader::Resolve(gfx, "PhongVS.cso");
    auto pvsbc = pvs->GetBytecode();
    bindablePtrs.push_back(std::move(pvs));

    bindablePtrs.push_back(Bind::PixelShader::Resolve(gfx, "PhongPSSpec.cso"));

    bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

    struct PSMaterialConstantDiffuseSpec
    {
      float specularPowerConst;
      BOOL hasGloss;
      float specularMapWeight;
      float padding;
    } pmc;
    pmc.specularPowerConst = shininess;
    pmc.hasGloss = hasAlphaGloss ? TRUE : FALSE;
    pmc.specularMapWeight = 1.0f;
    // this is CLEARLY an issue... all meshes will share same mat const, but may have different
    // Ns (specular power) specified for each in the material properties... bad conflict
    bindablePtrs.push_back(Bind::PixelConstantBuffer<PSMaterialConstantDiffuseSpec>::Resolve(gfx, pmc, 1u));
  }
  else if (hasDiffuseMap)
  {
    Dvtx::VertexBuffer vbuf(std::move(Dvtx::VertexLayout{}
      .Append(Dvtx::VertexLayout::Position3D)
      .Append(Dvtx::VertexLayout::Normal)
      .Append(Dvtx::VertexLayout::Texture2D)
    ));

    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
      vbuf.EmplaceBack(
        XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
        *reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
        *reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
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

    bindablePtrs.push_back(Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf));

    bindablePtrs.push_back(Bind::IndexBuffer::Resolve(gfx, meshTag, indices));

    auto pvs = Bind::VertexShader::Resolve(gfx, "PhongVS.cso");
    auto pvsbc = pvs->GetBytecode();
    bindablePtrs.push_back(std::move(pvs));

    bindablePtrs.push_back(Bind::PixelShader::Resolve(gfx, "PhongPS.cso"));

    bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

    struct PSMaterialConstantDiffuse
    {
      float specularIntensity;
      float specularPower;
      float padding[2];
    } pmc;

    pmc.specularPower = shininess;
    pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
    // this is CLEARLY an issue... all meshes will share same mat const, but may have different
    // Ns (specular power) specified for each in the material properties... bad conflict
    bindablePtrs.push_back(Bind::PixelConstantBuffer<PSMaterialConstantDiffuse>::Resolve(gfx, pmc, 1u));
  }
  else if (!hasDiffuseMap && !hasNormalMap && !hasSpecularMap)
  {
    Dvtx::VertexBuffer vbuf(std::move(Dvtx::VertexLayout{}
      .Append(Dvtx::VertexLayout::Position3D)
      .Append(Dvtx::VertexLayout::Normal)
    ));

    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
      vbuf.EmplaceBack(
        XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
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

    bindablePtrs.push_back(Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf));

    bindablePtrs.push_back(Bind::IndexBuffer::Resolve(gfx, meshTag, indices));

    auto pvs = Bind::VertexShader::Resolve(gfx, "PhongVSNotex.cso");
    auto pvsbc = pvs->GetBytecode();
    bindablePtrs.push_back(std::move(pvs));

    bindablePtrs.push_back(Bind::PixelShader::Resolve(gfx, "PhongPSNotex.cso"));

    bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

    Node::PSMaterialConstantNotex pmc;
    pmc.specularPower = shininess;
    pmc.specularColor = specularColor;
    pmc.materialColor = diffuseColor;
    // this is CLEARLY an issue... all meshes will share same mat const, but may have different
    // Ns (specular power) specified for each in the material properties... bad conflict
    bindablePtrs.push_back(Bind::PixelConstantBuffer<Node::PSMaterialConstantNotex>::Resolve(gfx, pmc, 1u));
  }
  else
  {
    throw std::runtime_error("terrible combination of textures in material smh");
  }

  return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
{
  const auto transform = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node.mTransformation)));

  std::vector<Mesh*> curMeshPtrs;
  curMeshPtrs.reserve(node.mNumMeshes);

  for (size_t i = 0; i < node.mNumMeshes; i++)
  {
    const auto meshIdx = node.mMeshes[i];
    curMeshPtrs.push_back(m_meshPtrs.at(meshIdx).get());
  }

  auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
  for (size_t i = 0; i < node.mNumChildren; i++)
    pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));

  return pNode;
}
