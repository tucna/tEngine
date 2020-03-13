#pragma once

#include <vector>
#include <Windows.h>

#include "Camera.h"
#include "Drawable.h"
#include "MainWindow.h"
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

  std::vector<std::unique_ptr<class Drawable>> m_drawables;
};

