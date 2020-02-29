#pragma once

#include <Windows.h>

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
};

