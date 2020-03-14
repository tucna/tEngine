#include "Common.h"

#include <string>

#include "App.h"
#include "Box.h"
#include "Sheet.h"

using namespace DirectX;

App::App() :
  m_window(800, 600, "Basic window"),
  m_light(m_window.GetGraphics())
{
  const DirectX::XMFLOAT3 material = {0.8f, 0.2f, 0.0f};
  m_drawables.push_back(std::make_unique<Box>(m_window.GetGraphics(), material));

  m_window.GetGraphics().SetProjection(XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 100.0f));
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
  const auto dt = m_timer.Mark() * 1;

  m_window.GetGraphics().BeginFrame(0.07f, 0.0f, 0.12f);

  m_window.GetGraphics().SetCamera(m_camera.GetMatrix());
  m_light.Bind(m_window.GetGraphics());

  for (auto& d : m_drawables)
  {
    d->Update(dt);
    d->Draw(m_window.GetGraphics());
  }

  m_window.GetGraphics().EndFrame();
}
