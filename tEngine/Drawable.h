#pragma once

#include "Common.h"

#include <memory>
#include <vector>

#include "Graphics.h"
#include "IndexBuffer.h"

namespace Bind
{
  class Bindable;
  class IndexBuffer;
}

class Drawable
{
public:
  Drawable() = default;
  Drawable(const Drawable&) = delete;

  virtual ~Drawable() = default;

  virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

  void Draw(Graphics& gfx) const noexcept;

  template<class T>
  T* QueryBindable() noexcept
  {
    for (auto& pb : m_binds)
    {
      if (auto pt = dynamic_cast<T*>(pb.get()))
        return pt;
    }

    return nullptr;
  }

protected:
  void AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept;

private:
  const class Bind::IndexBuffer* m_indexBuffer = nullptr;
  std::vector<std::shared_ptr<Bind::Bindable>> m_binds;
};
