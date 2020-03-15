#pragma once

#include "Common.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Bindable.h"
#include "DrawableBase.h"
#include "Vertex.h"

class Mesh : public DrawableBase<Mesh>
{
public:
  Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs);

  void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

  DirectX::XMMATRIX GetTransformXM() const noexcept override { return DirectX::XMLoadFloat4x4(&m_transform); }

private:
  mutable DirectX::XMFLOAT4X4 m_transform;
};

class Node
{
  friend class Model;

public:
  Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept;

  void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

  void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept {XMStoreFloat4x4(&m_appliedTransform, transform); }

  int GetId() const noexcept { return m_id; }

private:
  void AddChild(std::unique_ptr<Node> child) noexcept;

  DirectX::XMFLOAT4X4 m_transform;
  DirectX::XMFLOAT4X4 m_appliedTransform;

  std::string m_name;

  std::vector<std::unique_ptr<Node>> m_childPtrs;
  std::vector<Mesh*> m_meshPtrs;

  int m_id;
};

class Model
{
public:
  Model(Graphics& gfx, std::string_view fileName);

  ~Model() noexcept;

  void Draw(Graphics& gfx) const noexcept;

private:
  static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh);

  std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;

  std::unique_ptr<Node> m_root;
  std::unique_ptr<class ModelWindow> m_window;
  std::vector<std::unique_ptr<Mesh>> m_meshPtrs;
};
