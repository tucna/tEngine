#pragma once

#include "Common.h"

#include <vector>

#include "Camera.h"
#include "Drawable.h"
#include "MainWindow.h"
#include "Mesh.h"
#include "PointLight.h"
#include "Timer.h"

class App
{
public:
  App();

  int Go();

private:
  void DoFrame();

  Camera m_camera;
  Timer m_timer;
  MainWindow m_window;
  PointLight m_light;

  Keyboard& m_keyboard;
  Mouse& m_mouse;

  //Model m_suzanne;
  Model m_goblin;

  std::vector<std::unique_ptr<class Drawable>> m_drawables;
};
