#include "Common.h"

#include <string>

#include "App.h"
#include "Box.h"
#include "Sheet.h"

using namespace DirectX;

App::App() :
  m_window(windowWidth, windowHeight, "Basic window"),
  m_light(m_window.GetGraphics()),
  m_suzanne(m_window.GetGraphics(), "models\\suzanne.obj")
{
  //const DirectX::XMFLOAT3 material = {0.8f, 0.2f, 0.0f};
  //m_drawables.push_back(std::make_unique<Box>(m_window.GetGraphics(), material));

  m_window.GetGraphics().SetProjection(XMMatrixPerspectiveLH(1.0f, static_cast<float>(windowHeight) / static_cast<float>(windowWidth), 0.5f, 40.0f));
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

  /* TUCNA
  for (auto& d : m_drawables)
  {
    d->Update(dt);
    d->Draw(m_window.GetGraphics());
  }
  */

  m_suzanne.Draw(m_window.GetGraphics());

  while (const auto e = m_window.m_keyboard.ReadKey())
  {
    if (!e->IsPress())
    {
      continue;
    }

    switch (e->GetCode())
    {
    case VK_ESCAPE:
      if (m_window.CursorEnabled())
      {
        m_window.DisableCursor();
        m_window.m_mouse.EnableRaw();
      }
      else
      {
        m_window.EnableCursor();
        m_window.m_mouse.DisableRaw();
      }
      break;
    }
  }

  if (!m_window.CursorEnabled())
  {
    if (m_window.m_keyboard.KeyIsPressed('W'))
    {
      m_camera.Translate({ 0.0f,0.0f,dt });
    }
    if (m_window.m_keyboard.KeyIsPressed('A'))
    {
      m_camera.Translate({ -dt,0.0f,0.0f });
    }
    if (m_window.m_keyboard.KeyIsPressed('S'))
    {
      m_camera.Translate({ 0.0f,0.0f,-dt });
    }
    if (m_window.m_keyboard.KeyIsPressed('D'))
    {
      m_camera.Translate({ dt,0.0f,0.0f });
    }
    if (m_window.m_keyboard.KeyIsPressed('R'))
    {
      m_camera.Translate({ 0.0f,dt,0.0f });
    }
    if (m_window.m_keyboard.KeyIsPressed('F'))
    {
      m_camera.Translate({ 0.0f,-dt,0.0f });
    }
  }

  while (const auto delta = m_window.m_mouse.ReadRawDelta())
  {
    if (!m_window.CursorEnabled())
    {
      m_camera.Rotate((float)delta->x, (float)delta->y);
    }
  }

  m_window.GetGraphics().EndFrame();
}
