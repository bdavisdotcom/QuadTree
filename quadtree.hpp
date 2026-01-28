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

class QuadTree
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
    QuadTree *whichQuadContainsRect(const Rectangle &rect);

    // find the actual Quad that has extents that contain the supplied rectangle
    // AND which also contains the node (by the node's id) in the nodes list
    // this recursively searches down the tree
    QuadTree *findQuadContaingNodeIdByRect(int id, const Rectangle &rect);

    const Extents &getExtents() const;

    Quads getQuads() const;

    const std::vector<Node> &getNodes() const;

    int getNodeCount();

private:
    Extents extents;
    std::unique_ptr<QuadTree> topLeft;
    std::unique_ptr<QuadTree> topRight;
    std::unique_ptr<QuadTree> botRight;
    std::unique_ptr<QuadTree> botLeft;
    std::vector<Node> nodes;
    void splitQuads();
    bool _insertNode(const Node &node);
    void _collapseChildQuads();

    int id;

    static int nextId;
};

struct Quads
{
    Quads();

    QuadTree *topLeft;
    QuadTree *topRight;
    QuadTree *botRight;
    QuadTree *botLeft;
};
