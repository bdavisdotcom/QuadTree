#include "raylib.h"
#include "quadtree.hpp"
#include <vector>

void drawTree(const QuadTree &quadTree)
{
    auto extents = quadTree.getExtents();
    auto quads = quadTree.getQuads();

    DrawRectangleLines(extents.min.x, extents.min.y, extents.max.x - extents.min.x, extents.max.y - extents.min.y, RED);

    for (const Node &n : quadTree.getNodes())
    {
        DrawRectangleRec(n.rect, quads.topLeft ? PURPLE : GREEN); // PURPLE if node crosses quad boudaries
    }

    if (quads.topLeft)
    {
        drawTree(*quads.topLeft.get());
        drawTree(*quads.topRight.get());
        drawTree(*quads.botLeft.get());
        drawTree(*quads.botRight.get());
    }
}

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

    float timeAccum = GetTime();
    int nodeAddIndex = 0;
    bool doneBuildingTree = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (nodeAddIndex < NUMBER_OF_NODES)
        {
            if (nodeAddIndex >= NUMBER_OF_NODES)
            {
                break;
            }
            auto n = Node{nodeAddIndex, objects.at(nodeAddIndex)};
            quadTree.insertNode(n);
            nodeAddIndex++;
            if (nodeAddIndex >= NUMBER_OF_NODES)
            {
                doneBuildingTree = true;
                printf("\nDone building the tree.\n");
            }
        }

        drawTree(quadTree);

        EndDrawing();
    }

    auto testFind = [objects, quadTree](int id) mutable
    {
        const auto rect = objects.at(id);
        auto *qt = quadTree.findQuadContaingNodeIdByRect(id, rect);

        printf("\nid: %d found? %s", id, qt != nullptr ? "true" : "false");
        if (qt)
        {
            (*qt)->print();
        }
    };

    testFind(50);
    testFind(11);
    testFind(0);

    CloseWindow();

    return 0;
}
