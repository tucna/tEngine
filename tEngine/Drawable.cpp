#include "Drawable.h"
#include "IndexBuffer.h"

void Drawable::Draw(Graphics& gfx) const noexcept
{
  for (auto& b : m_binds)
    b->Bind(gfx);

  gfx.DrawIndexed(m_indexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept
{
  // Special case for index buffer
  if (typeid(*bind) == typeid(Bind::IndexBuffer))
  {
    assert("Binding multiple index buffers not allowed" && m_indexBuffer == nullptr);

    m_indexBuffer = &static_cast<Bind::IndexBuffer&>(*bind);
  }

  m_binds.push_back(std::move(bind));
}
