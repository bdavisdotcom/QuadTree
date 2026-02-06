#pragma once

#include "raylib.h"
#include <map>
#include <memory>
#include <vector>

const int MAX_QUAD_NODES = 2;

struct AABB
{
  AABB();
  AABB(const Vector2& min, const Vector2& max);
  Rectangle toRectangle() const;
  bool overlaps(const AABB& aabb) const;
  void print() const;
  Vector2 min;
  Vector2 max;
};

class QuadTree;

struct Node
{
  Node() = delete;
  Node(uint32_t id, const AABB& aabb, QuadTree* parent = nullptr);
  ~Node();
  uint32_t id;
  AABB aabb;
  QuadTree* parent;

  void print() const;

  static inline int deleteCounter = 0;
};

struct Quads;

class QuadTree
{
public:
  QuadTree() = delete;
  QuadTree(const Vector2& min, const Vector2& max);
  ~QuadTree();

  bool isWithinBoundary(const AABB& aabb);
  bool isPointWithinBoundary(const Vector2& point);
  bool insertNode(const std::shared_ptr<Node>& node);
  bool removeNode(uint32_t);
  bool moveNode(uint32_t id, const AABB& newAABB);
  int findNodeIndexAtThisLevel(uint32_t id);
  bool isLeaf() const;
  void print();

  // returns which Quad has extents (at this current level) which contain the
  // supplied rectangle. Nonrecursive.
  QuadTree* whichQuadContainsRect(const AABB& aabb);

  // find the actual Quad that has extents that contain the supplied rectangle
  // AND which also contains the node (by the node's id) in the nodes list
  // this recursively searches down the tree
  QuadTree* findQuadContaingNodeIdByRect(uint32_t id, const AABB& aabb);

  const AABB& getAABB() const;

  Quads getQuads() const;

  const std::vector<std::shared_ptr<Node>>& getNodes() const;

  size_t getNodeCount();
  size_t getNodeCountRecursive();

  bool checkAndAdjustQuads();

  void getCollisions(uint32_t id, std::vector<uint32_t>& collisions);
  void getCollisions(uint32_t id, const AABB& aabb,
                     std::vector<uint32_t>& collisions);

private:
  AABB extents;
  uint32_t _id;

  std::unique_ptr<QuadTree> topLeft;
  std::unique_ptr<QuadTree> topRight;
  std::unique_ptr<QuadTree> botRight;
  std::unique_ptr<QuadTree> botLeft;
  std::vector<std::shared_ptr<Node>> nodes;

  bool splitQuads();
  bool _insertNode(const std::shared_ptr<Node>& node);
  void _collapseChildQuads();

  inline static uint32_t nextId = 0;
  inline static std::map<uint32_t, std::shared_ptr<Node>> nodesMap =
      std::map<uint32_t, std::shared_ptr<Node>>();
};

struct Quads
{
  Quads();

  QuadTree* topLeft;
  QuadTree* topRight;
  QuadTree* botRight;
  QuadTree* botLeft;
};
