#include "DrawableBase.h"
#include "IndexBuffer.h"

template<class T>
void DrawableBase<T>::AddStaticBind(std::unique_ptr<Bindable> bind) noexcept
{
  assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));

  m_staticBinds.push_back(std::move(bind));
}

template<class T>
void DrawableBase<T>::AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept
{
  assert("Attempting to add index buffer a second time" && m_indexBuffer == nullptr);

  m_indexBuffer = ibuf.get();
  m_staticBinds.push_back(std::move(ibuf));
}

template<class T>
void DrawableBase<T>::SetIndexFromStatic() noexcept
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

template<class T>
const std::vector<std::unique_ptr<Bindable>>& DrawableBase<T>::GetStaticBinds() const noexcept
{
  return m_staticBinds;
}
