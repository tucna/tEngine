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
  DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
  mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
  friend class Model;

public:
  Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept;
  void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
  void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
  int GetId() const noexcept;
private:
  void AddChild(std::unique_ptr<Node> pChild) noexcept;
private:
  std::string name;
  int id;
  std::vector<std::unique_ptr<Node>> childPtrs;
  std::vector<Mesh*> meshPtrs;
  DirectX::XMFLOAT4X4 transform;
  DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
  Model(Graphics& gfx, const std::string fileName);
  void Draw(Graphics& gfx) const noexcept;
  ~Model() noexcept;
private:
  static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh);
  std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;
private:
  std::unique_ptr<Node> pRoot;
  std::vector<std::unique_ptr<Mesh>> meshPtrs;
  std::unique_ptr<class ModelWindow> pWindow;
};
