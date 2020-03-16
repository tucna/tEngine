#pragma once

#include <queue>
#include <optional>

class Mouse
{
  friend class MainWindow;

public:
  struct RawDelta
  {
    int32_t x;
    int32_t y;
  };

  class Event
  {
  public:
    enum class Type
    {
      LPress,
      LRelease,
      RPress,
      RRelease,
      WheelUp,
      WheelDown,
      Move,
      Enter,
      Leave,
    };

    Event(Type type, const Mouse& parent) noexcept :
      m_type(type),
      m_isLeftPressed(parent.m_isLeftPressed),
      m_isRightPressed(parent.m_isRightPressed),
      m_x(parent.m_x),
      m_y(parent.m_y)
    {}

    Type GetType() const noexcept { return m_type; }

    uint16_t GetPosX() const noexcept { return m_x; }
    uint16_t GetPosY() const noexcept { return m_y; }

    bool LeftIsPressed() const noexcept { return m_isLeftPressed; }
    bool RightIsPressed() const noexcept { return m_isRightPressed; }

    std::pair<uint16_t, uint16_t> GetPos() const noexcept { return{ m_x, m_y }; }

  private:
    Type m_type;

    bool m_isLeftPressed;
    bool m_isRightPressed;

    uint16_t m_x;
    uint16_t m_y;
  };

  Mouse();
  Mouse(const Mouse&) = delete;
  Mouse& operator=(const Mouse&) = delete;

  std::pair<uint16_t, uint16_t> GetPos() const noexcept { return { m_x, m_y }; }

  std::optional<RawDelta> ReadRawDelta() noexcept;
  std::optional<Mouse::Event> Read() noexcept;

  uint16_t GetPosX() const noexcept { return m_x; }
  uint16_t GetPosY() const noexcept { return m_y; }

  bool IsInWindow() const noexcept { return m_isInWindow; }
  bool IsLeftPressed() const noexcept { return m_isLeftPressed; }
  bool IsRightPressed() const noexcept { return m_isRightPressed; }
  bool IsEmpty() const noexcept { return m_buffer.empty(); }
  bool IsRawEnabled() const noexcept { return m_rawEnabled; }

  void EnableRaw() noexcept { m_rawEnabled = true; }
  void DisableRaw() noexcept { m_rawEnabled = false; }

  void Flush() noexcept { m_buffer = std::queue<Event>(); }

private:
  void OnMouseMove(uint16_t x, uint16_t y) noexcept;
  void OnMouseLeave() noexcept;
  void OnMouseEnter() noexcept;

  void OnLeftPressed(uint16_t x, uint16_t y) noexcept;
  void OnLeftReleased(uint16_t x, uint16_t y) noexcept;
  void OnRightPressed(uint16_t x, uint16_t y) noexcept;
  void OnRightReleased(uint16_t x, uint16_t y) noexcept;

  void OnWheelUp(uint16_t x, uint16_t y) noexcept;
  void OnWheelDown(uint16_t x, uint16_t y) noexcept;

  void TrimBuffer() noexcept;
  void TrimRawInputBuffer() noexcept;

  void OnWheelDelta(uint16_t x, uint16_t y, int16_t delta) noexcept;
  void OnRawDelta(int32_t dx, int32_t dy) noexcept;

  static constexpr uint16_t bufferSize = 16u;

  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_wheelDeltaCarry;

  bool m_isLeftPressed;
  bool m_isRightPressed;
  bool m_isInWindow;
  bool m_rawEnabled;

  std::queue<Event> m_buffer;
  std::queue<RawDelta> m_rawDeltaBuffer;
};
