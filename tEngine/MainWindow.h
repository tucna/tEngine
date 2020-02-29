#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <Windows.h>

#include "Graphics.h"

class MainWindow
{
public:
  MainWindow(uint16_t width, uint16_t height, std::string_view title) noexcept;
  ~MainWindow() noexcept;

  static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
  static std::optional<int> ProcessMessages();

  Graphics& GetGraphics() { return *m_graphics; }

private:
  static constexpr const char* wndClassName = "tEngine window";

  WNDCLASSEX m_wc;
  HWND m_hWnd;

  std::unique_ptr<Graphics> m_graphics;
};

