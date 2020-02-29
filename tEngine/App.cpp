#include <string>

#include "App.h"
#include "Common.h"

App::App() :
  m_window(640, 480, "Basic window")
{
}

int App::Go()
{
  while(true)
  {
    if (const auto code = MainWindow::ProcessMessages())
      return *code;

    DoFrame();
  }
}

void App::DoFrame()
{
  const float t = m_timer.Peek();
  LogNumber(t);
}
