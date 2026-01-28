#pragma once

#include <memory>
#include <vector>
#include "raylib.h"

const int MAX_QUAD_NODES = 8;

struct Extents
{
    Extents();
    Extents(Vector2 min, Vector2 max);
    Vector2 min;
    Vector2 max;
};

struct Node
{
    int id;
    Rectangle rect;

    void print() const;
};

struct Quads;

class QuadTree : public std::enable_shared_from_this<QuadTree>
{
public:
    QuadTree() = delete;
    QuadTree(const Vector2 &min, const Vector2 &max);
    ~QuadTree();

    bool isWithinBoundary(const Rectangle &rect);
    void insertNode(const Node &node);
    void removeNode(int id, const Rectangle &searchRect);
    int findNodeIndexAtThisLevel(int id);
    bool isLeaf() const;
    void print();

    // returns which Quad has extents (at this current level) which contain the supplied rectangle. Nonrecursive.
    std::shared_ptr<QuadTree> *whichQuadContainsRect(const Rectangle &rect);

    // find the actual Quad that has extents that contain the supplied rectangle
    // AND which also contains the node (by the node's id) in the nodes list
    // this recursively searches down the tree
    std::shared_ptr<QuadTree> *findQuadContaingNodeIdByRect(int id, const Rectangle &rect);

    const Extents &getExtents() const;

    Quads getQuads() const;

    const std::vector<Node> &getNodes() const;

    int getNodeCount();

private:
    Extents extents;
    std::shared_ptr<QuadTree> topLeft;
    std::shared_ptr<QuadTree> topRight;
    std::shared_ptr<QuadTree> botRight;
    std::shared_ptr<QuadTree> botLeft;
    std::vector<Node> nodes;
    void splitQuads();
    bool _insertNode(const Node &node);
    void _collapseChildQuads();

    std::shared_ptr<QuadTree> _sharedFromThis;
    int id;

    static int nextId;
};

struct Quads
{
    Quads();

    std::shared_ptr<QuadTree const> topLeft;
    std::shared_ptr<QuadTree const> topRight;
    std::shared_ptr<QuadTree const> botRight;
    std::shared_ptr<QuadTree const> botLeft;
};
