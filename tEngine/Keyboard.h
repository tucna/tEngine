#pragma once

#include <bitset>
#include <optional>
#include <queue>

class Keyboard
{
  friend class MainWindow;

public:
  class Event
  {
  public:
    enum class Type
    {
      Press,
      Release,
    };

    Event(Type type, uint8_t code) noexcept :
      m_type(type),
      m_code(code)
    {}

    bool IsPress() const noexcept { return m_type == Type::Press; }
    bool IsRelease() const noexcept { return m_type == Type::Release; }

    uint8_t GetCode() const noexcept { return m_code; }

  private:
    Type m_type;
    uint8_t m_code;
  };

  Keyboard();
  Keyboard(const Keyboard&) = delete;
  Keyboard& operator=(const Keyboard&) = delete;

  // Key event stuff
  bool IsKeyPressed(uint8_t keycode) const noexcept { return m_keystates[keycode]; }
  bool IsKeyEmpty() const noexcept { return m_keybuffer.empty(); }
  std::optional<Event> ReadKey() noexcept;
  void FlushKey() noexcept { m_keybuffer = std::queue<Event>(); }

  // Char event stuff
  bool IsCharEmpty() const noexcept { return m_charbuffer.empty(); }
  std::optional<char> ReadChar() noexcept;
  void FlushChar() noexcept { m_charbuffer = std::queue<uint8_t>(); }

  // Autorepeat control
  void EnableAutorepeat() noexcept { m_autorepeatEnabled = true; }
  void DisableAutorepeat() noexcept { m_autorepeatEnabled = false; }
  bool AutorepeatIsEnabled() const noexcept { return m_autorepeatEnabled; }

  void Flush() noexcept;

private:
  void OnKeyPressed(uint8_t keycode) noexcept;
  void OnKeyReleased(uint8_t keycode) noexcept;
  void OnChar(uint8_t character) noexcept;

  void ClearState() noexcept { m_keystates.reset(); }

  template<typename T>
  static void TrimBuffer(std::queue<T>& buffer) noexcept;

  static constexpr uint16_t nKeys = 256;
  static constexpr uint16_t bufferSize = 16;

  bool m_autorepeatEnabled;

  std::bitset<nKeys> m_keystates;

  std::queue<Event> m_keybuffer;
  std::queue<uint8_t> m_charbuffer;
};

