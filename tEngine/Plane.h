#pragma once

#include "IndexedTriangleList.h"

class Plane
{
public:
  template<class V>
  static IndexedTriangleList<V> MakeTesselated(int divisions_x, int divisions_y);

  template<class V>
  static IndexedTriangleList<V> Make();
};
