#include "aabb.hpp"

AABB::AABB() : min{0.0, 0.0}, max{0.0, 0.0} {}

AABB::AABB(const Vector2& min, const Vector2& max) : min(min), max(max) {}

Rectangle AABB::toRectangle() const
{
  return Rectangle{
      .x = min.x, .y = min.y, .width = max.x - min.x, .height = max.y - min.y};
}

bool AABB::overlaps(const AABB& aabb) const
{
  return !(aabb.max.x < min.x || aabb.min.x > max.x || aabb.max.y < min.y ||
           aabb.min.y > max.y);
}