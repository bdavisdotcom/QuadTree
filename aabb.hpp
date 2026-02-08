#pragma once

#include "raylib.h"

struct AABB
{
  AABB();
  AABB(const Vector2& min, const Vector2& max);

  Rectangle toRectangle() const;
  bool overlaps(const AABB& aabb) const;
  Vector2 min;
  Vector2 max;
};