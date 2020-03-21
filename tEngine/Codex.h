#pragma once

#include <memory>
#include <unordered_map>

#include "Graphics.h"

namespace Bind
{

class Codex
{
public:
  template<class T, typename...Params>
  static std::shared_ptr<T> Resolve(Graphics& gfx, Params&&...p) noexcept
  {
    static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");

    return Get().Resolve_<T>(gfx, std::forward<Params>(p)...);
  }

  template<class T, typename...Params>
  std::shared_ptr<T> Resolve_(Graphics& gfx, Params&&...p) noexcept
  {
    const auto key = T::GenerateUID(std::forward<Params>(p)...);
    const auto i = m_binds.find(key);

    if (i == m_binds.end())
    {
      auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
      m_binds[key] = bind;
      return bind;
    }
    else
    {
      return std::static_pointer_cast<T>(i->second);
    }
  }

  static Codex& Get()
  {
    static Codex codex;
    return codex;
  }

private:
  std::unordered_map<std::string, std::shared_ptr<Bindable>> m_binds;
};

}
