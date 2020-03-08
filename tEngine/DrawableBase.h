#pragma once

#include <memory>
#include <vector>

#include "Drawable.h"
#include "IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
  static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept;

  void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept;
  void SetIndexFromStatic() noexcept;

  static bool IsStaticInitialized() noexcept { return !m_staticBinds.empty(); }

private:
  const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override;

  static std::vector<std::unique_ptr<Bindable>> m_staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::m_staticBinds;
