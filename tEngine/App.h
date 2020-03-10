#pragma once

#include <vector>
#include <Windows.h>

#include "Drawable.h"
#include "MainWindow.h"
#include "Timer.h"

class App
{
public:
  App();

  int Go();

private:
  void DoFrame();

  MainWindow m_window;
  Timer m_timer;

  std::vector<std::unique_ptr<class Drawable>> m_drawables;
};

