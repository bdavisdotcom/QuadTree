# C++ QuadTree implementation

## Requires Raylib to run the test harness main.cpp

This is my experimental quadtree implementation written in C++. It has not yet been optimized--there are a few memory allocations and deallocations, as well as other things that could be optimized.

* It uses Raylib to visualize the tree with a moving rectangle.
* It highlights the quad that contains the moving node
* It will print out any collisions to the console as the rectangle travels across the screen.
* It will also visualize the subdivision of a quad into 4 nested quads, if the MAX_QUAD_NODES is exceeded (the moving node moves into a quad causing the number of nodes to grow past the max)
* The node is drawn in PURPLE if the node crosses a quad boundary
<img width="1055" height="830" alt="ksnip_20260205-210426" src="https://github.com/user-attachments/assets/34a94749-2cfa-4be1-9c98-f613aa805fb7" />
