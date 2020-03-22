#include "Common.h"

#include <string>

#include "App.h"

using namespace DirectX;

App::App() :
  m_window(windowWidth, windowHeight, "Basic window"),
  m_light(m_window.GetGraphics()),
  //m_suzanne(m_window.GetGraphics(), "models\\suzanne.obj"),
  m_goblin(m_window.GetGraphics(), "models\\goblin\\GoblinX.obj", 2),
  m_keyboard(m_window.GetKeyboard()),
  m_mouse(m_window.GetMouse())
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
  m_light.Bind(m_window.GetGraphics(), m_camera.GetMatrix());

  /*
  for (auto& d : m_drawables)
  {
    d->Update(dt);
    d->Draw(m_window.GetGraphics());
  }
  */

  //m_suzanne.Draw(m_window.GetGraphics());
  m_goblin.Draw(m_window.GetGraphics());
  m_light.Draw(m_window.GetGraphics());

  while (const auto e = m_window.GetKeyboard().ReadKey())
  {
    if (!e->IsPress())
      continue;

    switch (e->GetCode())
    {
    case VK_ESCAPE:
      if (m_window.CursorEnabled())
      {
        m_window.DisableCursor();
        m_mouse.EnableRaw();
      }
      else
      {
        m_window.EnableCursor();
        m_mouse.DisableRaw();
      }
      break;
    }
  }

  if (!m_window.CursorEnabled())
  {
    if (m_keyboard.IsKeyPressed('W'))
      m_camera.Translate({ 0.0f,0.0f,dt });

    if (m_keyboard.IsKeyPressed('A'))
      m_camera.Translate({ -dt,0.0f,0.0f });

    if (m_keyboard.IsKeyPressed('S'))
      m_camera.Translate({ 0.0f,0.0f,-dt });

    if (m_keyboard.IsKeyPressed('D'))
      m_camera.Translate({ dt,0.0f,0.0f });

    if (m_keyboard.IsKeyPressed('R'))
      m_camera.Translate({ 0.0f,dt,0.0f });

    if (m_keyboard.IsKeyPressed('F'))
      m_camera.Translate({ 0.0f,-dt,0.0f });
  }

  // Light
  if (m_keyboard.IsKeyPressed(102)) // 6
    m_light.Translate({ dt * 4,0.0f,0.0f });

  if (m_keyboard.IsKeyPressed(100)) // 4
    m_light.Translate({ -dt * 4,0.0f,0.0f });

  if (m_keyboard.IsKeyPressed(104)) // 8
    m_light.Translate({ 0.0f,0.0f,dt * 4 });

  if (m_keyboard.IsKeyPressed(101)) // 5
    m_light.Translate({ 0.0f,0.0f,-dt * 4 });

  if (m_keyboard.IsKeyPressed(105))
    m_light.Translate({ 0.0f,dt * 4,0.0f });

  if (m_keyboard.IsKeyPressed(103))
    m_light.Translate({ 0.0f,-dt * 4,0.0f });

  while (const auto delta = m_mouse.ReadRawDelta())
  {
    if (!m_window.CursorEnabled())
      m_camera.Rotate((float)delta->x, (float)delta->y);
  }

  m_window.GetGraphics().EndFrame();
}
