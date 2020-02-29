#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <Windows.h>

class MainWindow
{
public:
  MainWindow(uint16_t width, uint16_t height, std::string_view title) noexcept;
  ~MainWindow() noexcept;

  static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
  static std::optional<int> ProcessMessages();

private:
  static constexpr const char* wndClassName = "tEngine window";

  WNDCLASSEX wc;
  HWND hWnd;
};

