#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "quadtree.h"

extern bool shouldClose;

int createWindow(QuadTree* qt, int width);
void waitClosedWindow();

#endif // VISUALIZER_H