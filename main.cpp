#include "raylib.h"
#include "quadtree.hpp"
#include <vector>
#include <iostream>

void drawTree(const QuadTree *quadTree)
{
    auto extents = quadTree->getExtents();
    auto quads = quadTree->getQuads();

    DrawRectangleLines(extents.min.x, extents.min.y, extents.max.x - extents.min.x, extents.max.y - extents.min.y, RED);

    for (const Node &n : quadTree->getNodes())
    {
        DrawRectangleRec(n.rect, n.id == 0 ? BLUE : quads.topLeft ? PURPLE
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

void testFind(int id, const std::vector<Rectangle> &objects, QuadTree *quadTree)
{
    const auto rect = objects.at(id);
    auto *qt = quadTree->findQuadContaingNodeIdByRect(id, rect);

    printf("\nid: %d found? %s", id, qt != nullptr ? "true" : "false");
    if (qt)
    {
        qt->print();
    }
};

int main()
{
    SetTraceLogLevel(LOG_ERROR);

    const int NUMBER_OF_NODES = 400;
    const int NODE_SIZE = 5;
    const int screenWidth = 800;
    const int screenHeight = 600;

    auto quadTree = QuadTree(Vector2{0, 0}, Vector2{screenWidth, screenHeight});

    auto objects = std::vector<Rectangle>();

    objects.reserve(NUMBER_OF_NODES);

    for (int i = 0; i < NUMBER_OF_NODES; i++)
    {
        auto rect = Rectangle{(float)GetRandomValue(0, 800), (float)GetRandomValue(0, 600), NODE_SIZE, NODE_SIZE};
        objects.push_back(rect);
    }

    InitWindow(screenWidth, screenHeight, "raylib window example");

    SetTargetFPS(60);

    float timeAccum = 0;
    int nodeAddIndex = 0;
    int nodeRemoveIndex = objects.size() - 1;
    bool doneBuildingTree = false;
    bool done = false;
    Vector2 velocity = Vector2{20.0f, 20.0f};
    int keyPressed = 0;

    printf("\nobject count: %d", (int)objects.size());

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (nodeAddIndex < NUMBER_OF_NODES)
        {
            auto n = Node{nodeAddIndex, objects.at(nodeAddIndex)};
            quadTree.insertNode(n);
            nodeAddIndex++;
            if (nodeAddIndex >= NUMBER_OF_NODES)
            {
                doneBuildingTree = true;
                printf("\nDone building the tree.\n");
                printf("\nPress space key to start removing...");
            }
        }

        if (doneBuildingTree && nodeRemoveIndex > -1 && keyPressed == KEY_ENTER)
        {
            auto r = &objects.at(nodeRemoveIndex);
            bool res = quadTree.removeNode(nodeRemoveIndex, *r);
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

        drawTree(&quadTree);

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

    testFind(50, objects, &quadTree);
    testFind(11, objects, &quadTree);
    testFind(0, objects, &quadTree);

    CloseWindow();

    return 0;
}
