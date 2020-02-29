#include "Common.h"
#include "MainWindow.h"
#include "WindowsMessageMap.h"

MainWindow::MainWindow(uint16_t width, uint16_t height, std::string_view title) noexcept
{
  WNDCLASSEX wc = {};
  wc.cbSize = sizeof(wc);
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.hIcon = nullptr;
  wc.hCursor = nullptr;
  wc.hbrBackground = nullptr;
  wc.lpszMenuName = nullptr;
  wc.lpszClassName = wndClassName;
  wc.hIconSm = nullptr;

  RegisterClassEx(&wc);

  // Fix the window size based on the client size TODO

  hWnd = CreateWindowEx(
    0, wndClassName,
    title.data(),
    WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT, width, height,
    nullptr, nullptr, GetModuleHandle(nullptr), nullptr
  );

  ShowWindow(hWnd, SW_SHOW);
}

MainWindow::~MainWindow() noexcept
{
  UnregisterClass(wndClassName, GetModuleHandle(nullptr));
}

LRESULT MainWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
#if DEBUG_LEVEL > 0
  static WindowsMessageMap mm;
  OutputDebugString(mm(msg, lParam, wParam).c_str());
#endif

  switch (msg)
  {
  case WM_CLOSE:
    PostQuitMessage(42);
    break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

std::optional<int> MainWindow::ProcessMessages()
{
  // Message pump
  MSG msg = {};

  while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
  {
    if (msg.message == WM_QUIT)
      return msg.wParam;

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return {};
}
