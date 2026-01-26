#include "quadtree.hpp"
#include "raymath.h"
#include <stdio.h>
#include <stack>
#include <algorithm>

Extents::Extents() : min(0, 0), max(0, 0)
{
}

Extents::Extents(Vector2 min, Vector2 max) : min(min), max(max)
{
}

void Node::print() const
{
    printf("Node: id:%d x:%f y:%f w:%f h:%f", id, rect.x, rect.y, rect.width, rect.height);
}

QuadTree::QuadTree(const Vector2 &min, const Vector2 &max) : extents(min, max),
                                                             topLeft(std::shared_ptr<QuadTree>()),
                                                             topRight(std::shared_ptr<QuadTree>()),
                                                             botLeft(std::shared_ptr<QuadTree>()),
                                                             botRight(std::shared_ptr<QuadTree>()),
                                                             nodes()
{
    nodes.reserve(MAX_QUAD_NODES * 2);
}

bool QuadTree::isWithinBoundary(const Rectangle &rect)
{
    return rect.x >= extents.min.x && rect.x <= extents.max.x && rect.y >= extents.min.y && rect.y <= extents.max.y && rect.x + rect.width <= extents.max.x && rect.y + rect.height <= extents.max.y;
}

std::shared_ptr<QuadTree> QuadTree::whichQuadIsWithinBoundary(const Rectangle &rect)
{
    if (!topLeft)
    {
        return std::shared_ptr<QuadTree>();
    }

    if (topLeft->isWithinBoundary(rect))
    {
        return topLeft;
    }

    if (topRight->isWithinBoundary(rect))
    {
        return topRight;
    }

    if (botLeft->isWithinBoundary(rect))
    {
        return botLeft;
    }

    if (botRight->isWithinBoundary(rect))
    {
        return botRight;
    }

    return std::shared_ptr<QuadTree>();
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

void QuadTree::insertNode(const Node &node)
{
    if (node.id == -1)
    {
        return;
    }

    if (!isWithinBoundary(node.rect))
    {
        return;
    }

    // if this quad was already split, it's no longer a leaf node,
    // so we cannot store any nodes here... UNLESS this node would cross the boundaries of the children
    if (topLeft || topRight || botLeft || botRight)
    {
        if (_insertNode(node))
        {
            return;
        }
        printf("  node crosses child nodes' boundaries, so it must go here...");
        // if we get past this return, the node crosses the boundaries of the child nodes
        // so we have to let it go at this level, even though this region was already split
        // i.e., the child node rect MUST completely fix inside this quad's dimensions
    }

    // if there is still space here for this node
    // insert it and return
    if (nodes.size() < MAX_QUAD_NODES)
    {
        nodes.push_back(node);
        return;
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
}

int QuadTree::findNode(int id)
{
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

void QuadTree::_removeNodeAtThisLevel(int index)
{
    // swap and pop
    std::swap(nodes[index], nodes.back());
    nodes.pop_back();
    int nodeCount = nodes.size();

    if (nodeCount >= MAX_QUAD_NODES)
    {
        return;
    }

    // see if we *can* collapse the quad children...i.e., if collapsing and moving the children here would push it over the max node limit...
    if (topLeft && (nodeCount + topLeft->getNodeCount() + topRight->getNodeCount() + botLeft->getNodeCount() + botRight->getNodeCount()) >= MAX_QUAD_NODES)
    {
        return; // no, we can't collapse the children...
    }

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

void QuadTree::removeNode(int id)
{
    int index = findNode(id);
    // not found and we haven't subdivided this quad yet
    if (index == -1 && !topLeft)
    {
        return;
    }

    if (index >= 0)
    {
        return _removeNodeAtThisLevel(index);
    }

    const Node &node = nodes.at(index);

    // find which child quad this node belongs in
    std::shared_ptr<QuadTree> quadTree = whichQuadIsWithinBoundary(node.rect);
    if (!quadTree)
    {
        return; // didn't find any, bail...
    }

    // remove the node from the child quad we found above...
    quadTree->removeNode(id);
}

void QuadTree::splitQuads()
{
    printf("\nSPLITTING");
    print();

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
    printf(" Quad min x:%d y:%d | max x:%d y:%d\n", (int)extents.min.x, (int)extents.min.y, (int)extents.max.x, (int)extents.max.y);
}

Quads QuadTree::getQuads() const
{
    auto quads = Quads();

    if (topLeft)
    {
        quads.topLeft = topLeft;
        quads.topRight = topRight;
        quads.botLeft = botLeft;
        quads.botRight = botRight;
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