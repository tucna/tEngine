#include "Drawable.h"
#include "IndexBuffer.h"

void Drawable::Draw(Graphics& gfx) const noexcept
{
  for (auto& b : m_binds)
    b->Bind(gfx);

  for (auto& b : GetStaticBinds())
    b->Bind(gfx);

  gfx.DrawIndexed(m_indexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bind::Bindable> bind) noexcept
{
  assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(Bind::IndexBuffer));

  m_binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<class Bind::IndexBuffer> ibuf) noexcept
{
  assert("Attempting to add index buffer a second time" && m_indexBuffer == nullptr);

  m_indexBuffer = ibuf.get();
  m_binds.push_back(std::move(ibuf));
}
