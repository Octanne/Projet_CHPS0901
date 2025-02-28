#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "quadtree.h"

extern bool shouldClose;
extern bool shouldPause;
extern bool debugMode;

int createWindow(QuadTree* qt, double windowDefaultSize, double simulationSize);
void waitClosedWindow();

#endif // VISUALIZER_H