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

    bool isWithinBoundary(const Rectangle &rect);
    void insertNode(const Node &node);
    void removeNode(int id);
    int findNode(int id);
    void print();

    std::shared_ptr<QuadTree> whichQuadIsWithinBoundary(const Rectangle &rect);

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
    void _removeNodeAtThisLevel(int index);
};

struct Quads
{
    Quads();

    std::shared_ptr<QuadTree const> topLeft;
    std::shared_ptr<QuadTree const> topRight;
    std::shared_ptr<QuadTree const> botRight;
    std::shared_ptr<QuadTree const> botLeft;
};
