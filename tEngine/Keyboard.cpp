#include "Keyboard.h"

Keyboard::Keyboard() :
  m_autorepeatEnabled(false)
{
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
  if (m_keybuffer.size() > 0u)
  {
    Keyboard::Event e = m_keybuffer.front();
    m_keybuffer.pop();

    return e;
  }

  return {};
}

std::optional<char> Keyboard::ReadChar() noexcept
{
  if (m_charbuffer.size() > 0u)
  {
    uint8_t charcode = m_charbuffer.front();
    m_charbuffer.pop();

    return charcode;
  }

  return {};
}

void Keyboard::Flush() noexcept
{
  FlushKey();
  FlushChar();
}

void Keyboard::OnKeyPressed(uint8_t keycode) noexcept
{
  m_keystates[keycode] = true;
  m_keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));

  TrimBuffer(m_keybuffer);
}

void Keyboard::OnKeyReleased(uint8_t keycode) noexcept
{
  m_keystates[keycode] = false;
  m_keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));

  TrimBuffer(m_keybuffer);
}

void Keyboard::OnChar(uint8_t character) noexcept
{
  m_charbuffer.push(character);

  TrimBuffer(m_charbuffer);
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
  while (buffer.size() > bufferSize)
    buffer.pop();
}
