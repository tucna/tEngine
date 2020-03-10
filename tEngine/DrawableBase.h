#pragma once

#include <memory>
#include <vector>

#include "Drawable.h"
#include "IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
  static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept
  {
    assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));

    m_staticBinds.push_back(std::move(bind));
  }

  void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept
  {
    assert("Attempting to add index buffer a second time" && m_indexBuffer == nullptr);

    m_indexBuffer = ibuf.get();
    m_staticBinds.push_back(std::move(ibuf));
  }

  void SetIndexFromStatic() noexcept
  {
    assert("Attempting to add index buffer a second time" && m_indexBuffer == nullptr);

    for (const auto& b : m_staticBinds)
    {
      if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
      {
        m_indexBuffer = p;

        return;
      }
    }

    assert("Failed to find index buffer in static binds" && m_indexBuffer != nullptr);
  }

  static bool IsStaticInitialized() noexcept { return !m_staticBinds.empty(); }

private:
  const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override { return m_staticBinds; }

  static std::vector<std::unique_ptr<Bindable>> m_staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::m_staticBinds;
