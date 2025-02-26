#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "quadtree.h"

extern bool shouldClose;

int createWindow(QuadTree* qt, double windowSize = 1000, double scaleFactor = 10000);
void waitClosedWindow();

#endif // VISUALIZER_H