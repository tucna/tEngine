#include "Common.h"

#include "Mouse.h"

Mouse::Mouse() :
  m_isInWindow(false),
  m_isLeftPressed(false),
  m_isRightPressed(false),
  m_rawEnabled(false),
  m_wheelDeltaCarry(0)
{
}

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
{
  if (m_rawDeltaBuffer.empty())
    return std::nullopt;

  const RawDelta d = m_rawDeltaBuffer.front();
  m_rawDeltaBuffer.pop();

  return d;
}

std::optional<Mouse::Event> Mouse::Read() noexcept
{
  if (m_buffer.size() > 0u)
  {
    Mouse::Event e = m_buffer.front();
    m_buffer.pop();

    return e;
  }

  return {};
}

void Mouse::OnMouseMove(uint16_t x, uint16_t y) noexcept
{
  m_x = x;
  m_y = y;

  m_buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));

  TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
  m_isInWindow = false;
  m_buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));

  TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
  m_isInWindow = true;
  m_buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));

  TrimBuffer();
}

void Mouse::OnRawDelta(int32_t dx, int32_t dy) noexcept
{
  m_rawDeltaBuffer.push({ dx,dy });

  TrimBuffer();
}

void Mouse::OnLeftPressed(uint16_t x, uint16_t y) noexcept
{
  m_isLeftPressed = true;

  m_buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));

  TrimBuffer();
}

void Mouse::OnLeftReleased(uint16_t x, uint16_t y) noexcept
{
  m_isLeftPressed = false;

  m_buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));

  TrimBuffer();
}

void Mouse::OnRightPressed(uint16_t x, uint16_t y) noexcept
{
  m_isRightPressed = true;

  m_buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));

  TrimBuffer();
}

void Mouse::OnRightReleased(uint16_t x, uint16_t y) noexcept
{
  m_isRightPressed = false;

  m_buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));

  TrimBuffer();
}

void Mouse::OnWheelUp(uint16_t x, uint16_t y) noexcept
{
  m_buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));

  TrimBuffer();
}

void Mouse::OnWheelDown(uint16_t x, uint16_t y) noexcept
{
  m_buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));

  TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
  while (m_buffer.size() > bufferSize)
    m_buffer.pop();
}

void Mouse::TrimRawInputBuffer() noexcept
{
  while (m_rawDeltaBuffer.size() > bufferSize)
    m_rawDeltaBuffer.pop();
}

void Mouse::OnWheelDelta(uint16_t x, uint16_t y, int16_t delta) noexcept
{
  m_wheelDeltaCarry += delta;

  // Generate events for every 120
  while (m_wheelDeltaCarry >= WHEEL_DELTA)
  {
    m_wheelDeltaCarry -= WHEEL_DELTA;
    OnWheelUp(x, y);
  }

  while (m_wheelDeltaCarry <= -WHEEL_DELTA)
  {
    m_wheelDeltaCarry += WHEEL_DELTA;
    OnWheelDown(x, y);
  }
}
