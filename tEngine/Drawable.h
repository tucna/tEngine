#pragma once

#include "Common.h"

#include <memory>
#include <vector>

#include "Graphics.h"
#include "IndexBuffer.h"

class Bindable;

class Drawable
{
  template<class T>
  friend class DrawableBase;

public:
  Drawable() = default;
  Drawable(const Drawable&) = delete;

  virtual ~Drawable() = default;

  virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
  virtual void Update(float dt) noexcept = 0;

  void Draw(Graphics& gfx) const noexcept;

protected:
  void AddBind(std::unique_ptr<Bindable> bind) noexcept;
  void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;

private:
  virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

  const class IndexBuffer* m_indexBuffer = nullptr;
  std::vector<std::unique_ptr<Bindable>> m_binds;
};
