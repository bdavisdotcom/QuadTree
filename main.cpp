#include "raylib.h"
#include "raymath.h"
#include "quadtree.hpp"
#include <vector>
#include <iostream>

const int NUMBER_OF_NODES = 150;
const int NODE_SIZE = 5;
const int screenWidth = 800;
const int screenHeight = 600;

void drawTree(const QuadTree *quadTree)
{
    auto extents = quadTree->getAABB();
    auto quads = quadTree->getQuads();

    DrawRectangleLines(extents.min.x, extents.min.y, extents.max.x - extents.min.x, extents.max.y - extents.min.y, RED);

    const auto &nodes = quadTree->getNodes();
    for (const std::shared_ptr<Node> &n : nodes)
    {
        DrawRectangleRec(n->aabb.toRectangle(), n->id == 0 ? BLUE : quads.topLeft ? PURPLE
                                                                                  : GREEN); // PURPLE if node crosses quad boudaries
    }

    if (quads.topLeft)
    {
        drawTree(quads.topLeft);
        drawTree(quads.topRight);
        drawTree(quads.botLeft);
        drawTree(quads.botRight);
    }
}

void testFind(int id, const std::vector<AABB> &objects, QuadTree *quadTree)
{
    if (id >= objects.size())
    {
        return;
    }

    const auto aabb = objects.at(id);
    auto *qt = quadTree->findQuadContaingNodeIdByRect(id, aabb);

    printf("\nid: %d found? %s", id, qt != nullptr ? "true" : "false");
    if (qt)
    {
        qt->print();
    }
};

const AABB getRandomAABB()
{
    auto rx = (float)GetRandomValue(0, screenWidth);
    auto ry = (float)GetRandomValue(0, screenHeight);
    return AABB(Vector2{rx, ry}, Vector2{rx + NODE_SIZE, ry + NODE_SIZE});
}

void makeData(std::vector<AABB> &objects)
{
    // float x = 100;
    // float y = 100;

    // objects.push_back(AABB(Vector2{x, y}, Vector2{x + NODE_SIZE, y + NODE_SIZE}));

    // x = 50;
    // y = 50;
    // objects.push_back(AABB(Vector2{x, y}, Vector2{x + NODE_SIZE, y + NODE_SIZE}));

    // x = 700;
    // y = 50;
    // objects.push_back(AABB(Vector2{x, y}, Vector2{x + NODE_SIZE, y + NODE_SIZE}));

    // x = 50;
    // y = 500;
    // objects.push_back(AABB(Vector2{x, y}, Vector2{x + NODE_SIZE, y + NODE_SIZE}));

    // x = 700;
    // y = 500;
    // objects.push_back(AABB(Vector2{x, y}, Vector2{x + NODE_SIZE, y + NODE_SIZE}));

    for (int i = 0; i < NUMBER_OF_NODES; i++)
    {
        objects.push_back(getRandomAABB());
    }
}

int main()
{
    SetTraceLogLevel(LOG_ERROR);

    auto *quadTree = new QuadTree(Vector2{0, 0}, Vector2{screenWidth, screenHeight});

    auto objects = std::vector<AABB>();

    objects.reserve(NUMBER_OF_NODES);

    makeData(objects);

    InitWindow(screenWidth, screenHeight, "QuadTree example");

    SetTargetFPS(60);

    float timeAccum = 0;
    int nodeAddIndex = 0;
    int nodeRemoveIndex = objects.size() - 1;
    bool doneBuildingTree = false;
    bool moveMode = false;
    bool done = false;
    Vector2 velocity = Vector2{2.0f, 2.0f};
    int keyPressed = 0;

    printf("\nobject count: %d", (int)objects.size());

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (nodeAddIndex < NUMBER_OF_NODES)
        {
            auto n = std::make_shared<Node>(nodeAddIndex, objects.at(nodeAddIndex));
            quadTree->insertNode(n);
            nodeAddIndex++;
            if (nodeAddIndex == NUMBER_OF_NODES)
            {
                doneBuildingTree = true;
                moveMode = true;
                printf("\nDone building the tree.\n");
                printf("\nPress enter key to start removing...");
            }
        }

        if (doneBuildingTree && nodeRemoveIndex > -1 && keyPressed == KEY_ENTER)
        {
            bool res = quadTree->removeNode(nodeRemoveIndex);
            nodeRemoveIndex--;
        }

        if (nodeRemoveIndex == 0 && !done)
        {
            printf("\nDONE REMOVING...");
            done = true;
        }

        if (doneBuildingTree && timeAccum >= 1.0 && keyPressed == KEY_ENTER)
        {
            printf("\nNode count: %d", nodeRemoveIndex);
        }

        if (moveMode)
        {
            auto &obj = objects.at(0);
            obj.min = obj.min + velocity; //* (velocity * GetFrameTime());
            obj.max = obj.max + velocity;
            if (obj.min.x <= 0 || obj.min.x >= screenWidth)
            {
                velocity.x *= -1;
            }
            if (obj.min.y <= 0 || obj.min.y >= screenHeight)
            {
                velocity.y *= -1;
            }

            quadTree->moveNode(0, obj);
        }

        drawTree(quadTree);

        EndDrawing();

        if (timeAccum >= 1.0)
        {
            timeAccum = 0.0;
        }

        timeAccum += GetFrameTime();

        if (doneBuildingTree && IsKeyPressed(KEY_ENTER))
        {
            keyPressed = KEY_ENTER;
        }
    }

    delete quadTree;

    printf("\nTotal Node structs deleted: %d\n", Node::deleteCounter);

    CloseWindow();

    return 0;
}
