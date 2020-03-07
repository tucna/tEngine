#include "Common.h"

#include <string>

#include "App.h"

App::App() :
  m_window(800, 600, "Basic window")
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
  m_window.GetGraphics().Clear(0.5, 0, 0);
  m_window.GetGraphics().EndFrame();
}
