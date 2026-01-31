#pragma once

#include <memory>
#include <map>
#include <vector>
#include "raylib.h"

const int MAX_QUAD_NODES = 8;

struct AABB
{
    AABB();
    AABB(const Vector2 &min, const Vector2 &max);
    Rectangle toRectangle() const;

    Vector2 min;
    Vector2 max;
};

struct Node
{
    int id;
    AABB aabb;

    void print() const;
};

struct Quads;

class QuadTree
{
public:
    QuadTree() = delete;
    QuadTree(const Vector2 &min, const Vector2 &max);
    ~QuadTree();

    bool isWithinBoundary(const AABB &aabb);
    bool insertNode(const Node &node);
    bool removeNode(int id, const AABB &searchAABB);
    int findNodeIndexAtThisLevel(int id);
    bool isLeaf() const;
    void print();

    // returns which Quad has extents (at this current level) which contain the supplied rectangle. Nonrecursive.
    QuadTree *whichQuadContainsRect(const AABB &aabb);

    // find the actual Quad that has extents that contain the supplied rectangle
    // AND which also contains the node (by the node's id) in the nodes list
    // this recursively searches down the tree
    QuadTree *findQuadContaingNodeIdByRect(int id, const AABB &aabb);

    const AABB &getAABB() const;

    Quads getQuads() const;

    const std::vector<Node> &getNodes() const;

    int getNodeCount();

private:
    AABB extents;
    std::unique_ptr<QuadTree> topLeft;
    std::unique_ptr<QuadTree> topRight;
    std::unique_ptr<QuadTree> botRight;
    std::unique_ptr<QuadTree> botLeft;
    std::vector<Node> nodes;
    void splitQuads();
    bool _insertNode(const Node &node);
    void _collapseChildQuads();

    uint32_t id;

    inline static uint32_t nextId = 0;
    inline static std::map<uint32_t, std::shared_ptr<AABB>> _nodes = std::map<uint32_t, std::shared_ptr<AABB>>();
};

struct Quads
{
    Quads();

    QuadTree *topLeft;
    QuadTree *topRight;
    QuadTree *botRight;
    QuadTree *botLeft;
};
