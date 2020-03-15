#include "Common.h"

#include "MainWindow.h"
#include "WindowsMessageMap.h"

MainWindow::MainWindow(uint16_t width, uint16_t height, std::string_view title) noexcept
{
  m_wc = {};
  m_wc.cbSize = sizeof(m_wc);
  m_wc.style = CS_OWNDC;
  m_wc.lpfnWndProc = WndProc;
  m_wc.cbClsExtra = 0;
  m_wc.cbWndExtra = 0;
  m_wc.hInstance = GetModuleHandle(nullptr);
  m_wc.hIcon = nullptr;
  m_wc.hCursor = nullptr;
  m_wc.hbrBackground = nullptr;
  m_wc.lpszMenuName = nullptr;
  m_wc.lpszClassName = wndClassName;
  m_wc.hIconSm = nullptr;

  RegisterClassEx(&m_wc);

  // Fix the window size based on the client size TODO

  m_hWnd = CreateWindowEx(
    0, wndClassName,
    title.data(),
    WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT, width, height,
    nullptr, nullptr, GetModuleHandle(nullptr), nullptr
  );

  ShowWindow(m_hWnd, SW_SHOW);

  m_graphics = std::make_unique<Graphics>(m_hWnd, width, height);
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
      return static_cast<int>(msg.wParam);

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return {};
}
