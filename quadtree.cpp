#include "quadtree.hpp"
#include "raymath.h"
#include <stdio.h>
#include <stack>
#include <algorithm>

Extents::Extents() : min{0.0, 0.0}, max{0.0, 0.0}
{
}

Extents::Extents(Vector2 min, Vector2 max) : min(min), max(max)
{
}

void Node::print() const
{
    printf("\nNode: id:%d x:%f y:%f w:%f h:%f", id, rect.x, rect.y, rect.width, rect.height);
}

int QuadTree::nextId = 0;

QuadTree::QuadTree(const Vector2 &min, const Vector2 &max) : extents(min, max),
                                                             topLeft(std::unique_ptr<QuadTree>()),
                                                             topRight(std::unique_ptr<QuadTree>()),
                                                             botLeft(std::unique_ptr<QuadTree>()),
                                                             botRight(std::unique_ptr<QuadTree>()),
                                                             nodes()
{
    nodes.reserve(MAX_QUAD_NODES * 2);
    id = QuadTree::nextId++;
}

QuadTree::~QuadTree()
{
    printf("\n*** QuadTree dtor(%d) -- ", id);
    print();
}

bool QuadTree::isLeaf() const
{
    // if any of the child quads are present, it's not a leaf (there will be all 4 quads or NONE)
    if (topLeft)
    {
        return false;
    }

    return true;
}

bool QuadTree::isWithinBoundary(const Rectangle &rect)
{
    return rect.x >= extents.min.x && rect.x <= extents.max.x && rect.y >= extents.min.y && rect.y <= extents.max.y && rect.x + rect.width <= extents.max.x && rect.y + rect.height <= extents.max.y;
}

QuadTree *QuadTree::whichQuadContainsRect(const Rectangle &rect)
{
    if (isLeaf())
    {
        return nullptr;
    }

    if (topLeft->isWithinBoundary(rect))
    {
        return topLeft.get();
    }

    if (topRight->isWithinBoundary(rect))
    {
        return topRight.get();
    }

    if (botLeft->isWithinBoundary(rect))
    {
        return botLeft.get();
    }

    if (botRight->isWithinBoundary(rect))
    {
        return botRight.get();
    }

    return nullptr;
}

QuadTree *QuadTree::findQuadContaingNodeIdByRect(int id, const Rectangle &rect)
{
    // is the node here in this nodes list?
    if (nodes.size() > 0)
    {
        int index = findNodeIndexAtThisLevel(id);
        if (index > -1)
        {
            return this;
        }
    }

    // is this a leaf node?
    if (isLeaf())
    {
        return nullptr;
    }

    // find out which quad potentially contains the node id passed
    auto *quadTree = whichQuadContainsRect(rect);
    if (!quadTree)
    {
        return nullptr;
    }

    quadTree->print();

    return quadTree->findQuadContaingNodeIdByRect(id, rect);
}

// private child quad node inserter
bool QuadTree::_insertNode(const Node &node)
{
    if (topLeft->isWithinBoundary(node.rect))
    {
        topLeft->insertNode(node);
        return true;
    }
    if (topRight->isWithinBoundary(node.rect))
    {
        topRight->insertNode(node);
        return true;
    }
    if (botLeft->isWithinBoundary(node.rect))
    {
        botLeft->insertNode(node);
        return true;
    }
    if (botRight->isWithinBoundary(node.rect))
    {
        botRight->insertNode(node);
        return true;
    }

    return false;
}

bool QuadTree::insertNode(const Node &node)
{
    if (node.id < 0)
    {
        return false;
    }

    if (!isWithinBoundary(node.rect))
    {
        return false;
    }

    // if this quad was already split, it's no longer a leaf node,
    // so we cannot store any nodes here... UNLESS this node would cross the boundaries of the children
    if (!isLeaf())
    {
        if (_insertNode(node))
        {
            return true;
        }
        // if we get past this return, the node crosses the boundaries of the child nodes
        // so we have to let it go at this level, even though this region was already split
        // i.e., the child node rect MUST completely fix inside this quad's dimensions
    }

    // if there is still space here for this node
    // insert it and return
    if (nodes.size() < MAX_QUAD_NODES)
    {
        nodes.push_back(node);
        return true;
    }

    // ...otherwise, there's no room left, so, split the region into 4 child quads
    // figure out where the new node belongs...

    // subdivide this quad into 4 children...
    splitQuads();

    auto cantMoveStack = std::stack<Node>();

    for (const Node &moveNode : nodes)
    {
        if (!_insertNode(moveNode))
        {
            cantMoveStack.push(moveNode);
        }
    }

    nodes.clear();

    // try to insert the node that caused the split in the first place...
    // if it can't be inserted (border crosser), then also just add it here...
    if (!_insertNode(node))
    {
        cantMoveStack.push(node);
    }

    // items in the "can't move stack" were probably border crossers
    // so they belong here in this "parent" quad instead of the subdivided child quads...
    // pop them off the stack, then re-add them to the nodes
    // this could potentially add more nodes than our MAX_QUAD_NODES const
    // but we have no choice in this case... this is why we reserve 2x the MAX_QUAD_NODES for the std::vector
    while (cantMoveStack.size() > 0)
    {
        nodes.push_back(cantMoveStack.top());
        cantMoveStack.pop();
    }

    return true;
}

int QuadTree::findNodeIndexAtThisLevel(int id)
{
    if (nodes.size() == 0)
    {
        return -1;
    }

    auto it = std::find_if(nodes.begin(),
                           nodes.end(),
                           [id](const Node &n)
                           { return n.id == id; });

    if (it == nodes.end())
    {
        return -1;
    }

    return std::distance(nodes.begin(), it);
}

int QuadTree::getNodeCount()
{
    return nodes.size();
}

void QuadTree::_collapseChildQuads()
{
    // copy nodes from child quads
    const auto &tlSrc = topLeft->getNodes();
    const auto &trSrc = topRight->getNodes();
    const auto &blSrc = botLeft->getNodes();
    const auto &brSrc = botRight->getNodes();

    nodes.insert(nodes.end(), tlSrc.begin(), tlSrc.end());
    nodes.insert(nodes.end(), trSrc.begin(), trSrc.end());
    nodes.insert(nodes.end(), blSrc.begin(), blSrc.end());
    nodes.insert(nodes.end(), brSrc.begin(), brSrc.end());

    // reset / remove the childquads
    topLeft.reset();
    topRight.reset();
    botLeft.reset();
    botRight.reset();
}

bool QuadTree::removeNode(int id, const Rectangle &searchRect)
{
    int index = findNodeIndexAtThisLevel(id);
    // not found and we haven't subdivided this quad yet
    if (index == -1 && isLeaf())
    {
        return false;
    }

    // found it at this level
    if (index >= 0)
    {
        //  swap and pop
        std::swap(nodes[index], nodes.back());
        nodes.pop_back();
        return true;
    }

    // if we get here, we didn't find it on this level
    // but we *may* have 4 child quads to check

    // no child quads, bail out
    if (isLeaf())
    {
        return false;
    }

    // find which child should contain the rect
    auto *quadTree = whichQuadContainsRect(searchRect);
    if (!quadTree)
    {
        // couldn't find a quad that contains this rect, so bail
        return false;
    }

    // remove the node from the child quad we found above...
    quadTree->removeNode(id, searchRect);

    // after this point we already return true, since the node was already removed
    // the rest of the steps are for optimization

    // check if we have to rebalance this QuadTree
    if (nodes.size() + topLeft->getNodeCount() + topRight->getNodeCount() + botLeft->getNodeCount() + botRight->getNodeCount() >= MAX_QUAD_NODES)
    {
        return true; // no, we can't collapse the child quads...
    }

    // if any of the child quads also have child quads, then we cannot collapse the children
    if (!topLeft->isLeaf() || !topRight->isLeaf() || !botLeft->isLeaf() || !botRight->isLeaf())
    {
        return true;
    }

    // if we reach this point, we *can* collapse the child quads...
    _collapseChildQuads();

    return true;
}

void QuadTree::splitQuads()
{
    Vector2 midTop = Vector2{extents.min.x + ((extents.max.x - extents.min.x) / 2), extents.min.y};
    Vector2 midMid = Vector2{midTop.x, extents.min.y + ((extents.max.y - extents.min.y) / 2)};
    Vector2 midBot = Vector2{midTop.x, extents.max.y};

    topLeft.reset(new QuadTree(extents.min, midMid));
    topRight.reset(new QuadTree(midTop, Vector2{extents.max.x, midMid.y}));
    botLeft.reset(new QuadTree(Vector2{extents.min.x, midMid.y}, midBot));
    botRight.reset(new QuadTree(midMid, extents.max));
}

void QuadTree::print()
{
    printf("\nQuad min x:%d y:%d | max x:%d y:%d\n", (int)extents.min.x, (int)extents.min.y, (int)extents.max.x, (int)extents.max.y);
}

Quads QuadTree::getQuads() const
{
    auto quads = Quads();

    if (!isLeaf())
    {
        quads.topLeft = topLeft.get();
        quads.topRight = topRight.get();
        quads.botLeft = botLeft.get();
        quads.botRight = botRight.get();
    }

    return quads;
}

const std::vector<Node> &QuadTree::getNodes() const
{
    return nodes;
}

const Extents &QuadTree::getExtents() const
{
    return extents;
}

Quads::Quads() : topLeft(nullptr), topRight(nullptr), botLeft(nullptr), botRight(nullptr)
{
}