#include "barnes_hut_seq.h"

#include "quadtree.h"
#include "visualizer.h"

#include <iostream>

int main(int argc, char** argv) {
    std::cout << "Barnes-Hut Sequential Implementation" << std::endl;
    // We add fives particles to the simulation
    Particle *p1 = new Particle(100.0, 684.0, 100.0);
    Particle *p2 = new Particle(487.0, 488.0, 100.0);
    Particle *p3 = new Particle(254.0, 548.0, 100.0);
    Particle *p4 = new Particle(324.0, 178.0, 100.0);
    Particle *p5 = new Particle(458.0, 125.0, 100.0);

    // We create a quadtree with a width and height of 150.0
    QuadTree qt;

    // We insert the particles into the quadtree
    qt.insert(p1);
    qt.insert(p2);
    qt.insert(p3);
    qt.insert(p4);
    qt.insert(p5);

    // We print the quadtree structure in a window
    createWindow(&qt);

    std::cout << "Press any key to close the window" << std::endl;
    waitClosedWindow();

    // We clear the quadtree
    qt.clear();

    // We delete the particles
    delete p1;
    delete p2;
    delete p3;
    delete p4;
    delete p5;

    std::cout << "End of Barnes-Hut Sequential Implementation" << std::endl;

    return 0;
}