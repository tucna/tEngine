#include "Common.h"

#include <random>
#include <string>

#include "App.h"
#include "Sheet.h"

using namespace DirectX;

App::App() :
  m_window(800, 600, "Basic window")
{
  std::mt19937 rng{ std::random_device{}() };
  std::uniform_real_distribution<float> adist{ 0.0f,3.14f * 2.0f };
  std::uniform_real_distribution<float> ddist{ 0.0f,3.14f * 0.5f };
  std::uniform_real_distribution<float> odist{ 0.0f,3.14f * 0.08f };
  std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
  std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
  std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };

  m_drawables.push_back(std::make_unique<Sheet>(m_window.GetGraphics(), rng, adist, ddist, odist, rdist));

  m_window.GetGraphics().SetProjection(XMMatrixPerspectiveLH(800, 600, 0.5f, 100.0f));
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

  m_window.GetGraphics().Clear(0, 0, 0);
  m_window.GetGraphics().SetCamera(m_camera.GetMatrix());

  for (auto& d : m_drawables)
  {
    d->Update(dt);
    d->Draw(m_window.GetGraphics());
  }

  m_window.GetGraphics().EndFrame();
}
