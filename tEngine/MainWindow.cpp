#include "Common.h"

#include "MainWindow.h"
#include "WindowsMessageMap.h"

// Window Class Stuff
MainWindow::WindowClass MainWindow::WindowClass::wndClass;

MainWindow::WindowClass::WindowClass() noexcept :
  hInst(GetModuleHandle(nullptr))
{
  WNDCLASSEX wc = { 0 };
  wc.cbSize = sizeof(wc);
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = HandleMsgSetup;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = GetInstance();
  wc.hIcon = static_cast<HICON>(LoadImage(
    GetInstance(), nullptr/*MAKEINTRESOURCE(IDI_ICON1) TUCNA*/,
    IMAGE_ICON, 32, 32, 0
  ));
  wc.hCursor = nullptr;
  wc.hbrBackground = nullptr;
  wc.lpszMenuName = nullptr;
  wc.lpszClassName = GetName();
  wc.hIconSm = static_cast<HICON>(LoadImage(
    GetInstance(), nullptr/*MAKEINTRESOURCE(IDI_ICON1) TUCNA*/,
    IMAGE_ICON, 16, 16, 0
  ));
  RegisterClassEx(&wc);
}

MainWindow::WindowClass::~WindowClass()
{
  UnregisterClass(wndClassName, GetInstance());
}

const char* MainWindow::WindowClass::GetName() noexcept
{
  return wndClassName;
}

HINSTANCE MainWindow::WindowClass::GetInstance() noexcept
{
  return wndClass.hInst;
}

MainWindow::MainWindow(uint16_t width, uint16_t height, std::string_view title) noexcept :
  m_cursorEnabled(true),
  m_width(width),
  m_height(height)
{
  // calculate window size based on desired client region size
  RECT wr;
  wr.left = 100;
  wr.right = width + wr.left;
  wr.top = 100;
  wr.bottom = height + wr.top;

  AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

  // create window & get hWnd
  m_hWnd = CreateWindow(
    WindowClass::GetName(), title.data(),
    WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
    nullptr, nullptr, WindowClass::GetInstance(), this
  );

  // newly created windows start off as hidden
  ShowWindow(m_hWnd, SW_SHOWDEFAULT);

  // create graphics object
  m_graphics = std::make_unique<Graphics>(m_hWnd, width, height);

  // register mouse raw input device
  RAWINPUTDEVICE rid;
  rid.usUsagePage = 0x01; // mouse page
  rid.usUsage = 0x02; // mouse usage
  rid.dwFlags = 0;
  rid.hwndTarget = nullptr;

  RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

MainWindow::~MainWindow() noexcept
{
  DestroyWindow(m_hWnd);
}

LRESULT MainWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
  // use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
  if (msg == WM_NCCREATE)
  {
    // extract ptr to window class from creation data
    const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
    MainWindow* const pWnd = static_cast<MainWindow*>(pCreate->lpCreateParams);
    // set WinAPI-managed user data to store ptr to window instance
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
    // set message proc to normal (non-setup) handler now that setup is finished
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&MainWindow::HandleMsgThunk));
    // forward message to window instance handler
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
  }
  // if we get a message before the WM_NCCREATE message, handle with default handler
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT MainWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
  // retrieve ptr to window instance
  MainWindow* const pWnd = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  // forward message to window instance handler
  return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT MainWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
#if DEBUG_LEVEL > 0
  static WindowsMessageMap mm;
  OutputDebugString(mm(msg, lParam, wParam).c_str());
#endif

  switch (msg)
  {
    // we don't want the DefProc to handle this message because
    // we want our destructor to destroy the window, so return 0 instead of break
  case WM_CLOSE:
    PostQuitMessage(0);
    return 0;
    // clear keystate when window loses focus to prevent input getting "stuck"
  case WM_KILLFOCUS:
    m_keyboard.ClearState();
    break;
  case WM_ACTIVATE:
    // confine/free cursor on window to foreground/background if cursor disabled
    if (!m_cursorEnabled)
    {
      if (wParam & WA_ACTIVE)
      {
        ConfineCursor();
        HideCursor();
      }
      else
      {
        FreeCursor();
        ShowCursor();
      }
    }
    break;

    /*********** KEYBOARD MESSAGES ***********/
  case WM_KEYDOWN:
    // syskey commands need to be handled to track ALT key (VK_MENU) and F10
  case WM_SYSKEYDOWN:
    if (!(lParam & 0x40000000) || m_keyboard.AutorepeatIsEnabled()) // filter autorepeat
    {
      m_keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
    }
    break;
  case WM_KEYUP:
  case WM_SYSKEYUP:
    m_keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
    break;
  case WM_CHAR:
    m_keyboard.OnChar(static_cast<unsigned char>(wParam));
    break;
    /*********** END KEYBOARD MESSAGES ***********/

    /************* MOUSE MESSAGES ****************/
  case WM_MOUSEMOVE:
  {
    const POINTS pt = MAKEPOINTS(lParam);
    // cursorless exclusive gets first dibs
    if (!m_cursorEnabled)
    {
      if (!m_mouse.IsInWindow())
      {
        SetCapture(hWnd);
        m_mouse.OnMouseEnter();
        HideCursor();
      }
      break;
    }
    // in client region -> log move, and log enter + capture mouse (if not previously in window)
    if (pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height)
    {
      m_mouse.OnMouseMove(pt.x, pt.y);
      if (!m_mouse.IsInWindow())
      {
        SetCapture(hWnd);
        m_mouse.OnMouseEnter();
      }
    }
    // not in client -> log move / maintain capture if button down
    else
    {
      if (wParam & (MK_LBUTTON | MK_RBUTTON))
      {
        m_mouse.OnMouseMove(pt.x, pt.y);
      }
      // button up -> release capture / log event for leaving
      else
      {
        ReleaseCapture();
        m_mouse.OnMouseLeave();
      }
    }
    break;
  }
  case WM_LBUTTONDOWN:
  {
    SetForegroundWindow(hWnd);
    if (!m_cursorEnabled)
    {
      ConfineCursor();
      HideCursor();
    }
    const POINTS pt = MAKEPOINTS(lParam);
    m_mouse.OnLeftPressed(pt.x, pt.y);
    break;
  }
  case WM_RBUTTONDOWN:
  {
    const POINTS pt = MAKEPOINTS(lParam);
    m_mouse.OnRightPressed(pt.x, pt.y);
    break;
  }
  case WM_LBUTTONUP:
  {
    const POINTS pt = MAKEPOINTS(lParam);
    m_mouse.OnLeftReleased(pt.x, pt.y);
    // release mouse if outside of window
    if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
    {
      ReleaseCapture();
      m_mouse.OnMouseLeave();
    }
    break;
  }
  case WM_RBUTTONUP:
  {
    const POINTS pt = MAKEPOINTS(lParam);
    m_mouse.OnRightReleased(pt.x, pt.y);
    // release mouse if outside of window
    if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
    {
      ReleaseCapture();
      m_mouse.OnMouseLeave();
    }
    break;
  }
  case WM_MOUSEWHEEL:
  {
    const POINTS pt = MAKEPOINTS(lParam);
    const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    m_mouse.OnWheelDelta(pt.x, pt.y, delta);
    break;
  }
  /************** END MOUSE MESSAGES **************/

  /************** RAW MOUSE MESSAGES **************/
  case WM_INPUT:
  {
    if (!m_mouse.RawEnabled())
    {
      break;
    }
    UINT size;
    // first get the size of the input data
    if (GetRawInputData(
      reinterpret_cast<HRAWINPUT>(lParam),
      RID_INPUT,
      nullptr,
      &size,
      sizeof(RAWINPUTHEADER)) == -1)
    {
      // bail msg processing if error
      break;
    }
    m_rawBuffer.resize(size);
    // read in the input data
    if (GetRawInputData(
      reinterpret_cast<HRAWINPUT>(lParam),
      RID_INPUT,
      m_rawBuffer.data(),
      &size,
      sizeof(RAWINPUTHEADER)) != size)
    {
      // bail msg processing if error
      break;
    }
    // process the raw input data
    auto& ri = reinterpret_cast<const RAWINPUT&>(*m_rawBuffer.data());
    if (ri.header.dwType == RIM_TYPEMOUSE &&
      (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
    {
      m_mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
    }
    break;
  }
  /************** END RAW MOUSE MESSAGES **************/
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

void MainWindow::EnableCursor() noexcept
{
  m_cursorEnabled = true;

  ShowCursor();
  FreeCursor();
}

void MainWindow::DisableCursor() noexcept
{
  m_cursorEnabled = false;

  HideCursor();
  ConfineCursor();
}

bool MainWindow::CursorEnabled() const noexcept
{
  return m_cursorEnabled;
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

void MainWindow::ConfineCursor() noexcept
{
  RECT rect;

  GetClientRect(m_hWnd, &rect);
  MapWindowPoints(m_hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
  ClipCursor(&rect);
}

void MainWindow::FreeCursor() noexcept
{
  ClipCursor(nullptr);
}

void MainWindow::HideCursor() noexcept
{
  while (::ShowCursor(FALSE) >= 0);
}

void MainWindow::ShowCursor() noexcept
{
  while (::ShowCursor(TRUE) < 0);
}
