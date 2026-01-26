#!/bin/bash

export LD_LIBRARY_PATH=/usr/local/lib/raylib/lib
g++ -std=c++23 -c main.cpp quadtree.cpp -I /usr/local/lib/raylib/include
g++ main.o quadtree.o -L /usr/local/lib/raylib/lib -l raylib -o tutorial