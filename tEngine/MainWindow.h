#pragma once

#include "Common.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"

class MainWindow
{
public:
  MainWindow(uint16_t width, uint16_t height, std::string_view title) noexcept;
  ~MainWindow() noexcept;

  static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
  static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

  static std::optional<int> ProcessMessages();

  LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

  void EnableCursor() noexcept;
  void DisableCursor() noexcept;
  bool CursorEnabled() const noexcept;

  Graphics& GetGraphics() { return *m_graphics; }

  Keyboard& GetKeyboard() { return m_keyboard; }
  Mouse& GetMouse() { return m_mouse; }

private:
  class WindowClass
  {
  public:
    static const char* GetName() noexcept;
    static HINSTANCE GetInstance() noexcept;
  private:
    WindowClass() noexcept;
    ~WindowClass();

    WindowClass(const WindowClass&) = delete;
    WindowClass& operator=(const WindowClass&) = delete;
    static constexpr const char* wndClassName = "tEngine window";
    static WindowClass wndClass;
    HINSTANCE hInst;
  };

  void ConfineCursor() noexcept;
  void FreeCursor() noexcept;
  void ShowCursor() noexcept;
  void HideCursor() noexcept;

  WNDCLASSEX m_wc;
  HWND m_hWnd;

  std::unique_ptr<Graphics> m_graphics;

  std::vector<uint8_t> m_rawBuffer;

  bool m_cursorEnabled;

  uint16_t m_width;
  uint16_t m_height;

  Mouse m_mouse;
  Keyboard m_keyboard;
};

