#!/bin/bash

g++ -std=c++23 -c *.cpp -I /usr/local/include
g++ ./*.o -L /usr/local/lib -l raylib -o tutorial