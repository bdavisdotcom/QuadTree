#include "node.hpp"

Node::Node(uint32_t id, const AABB& aabb, QuadTree* parent)
    : id(id), aabb(aabb), parent(parent)
{
}

Node::~Node() { ++deleteCounter; }
