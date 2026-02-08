#pragma once

#include "aabb.hpp"
#include <cstdint>

class QuadTree;

struct Node
{
  Node() = delete;
  Node(uint32_t id, const AABB& aabb, QuadTree* parent = nullptr);
  ~Node();
  uint32_t id;
  AABB aabb;
  QuadTree* parent;

  static inline int deleteCounter = 0;
};