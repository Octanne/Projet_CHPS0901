#include "barnes_hut_seq.h"

#include "quadtree.h"
#include "visualizer.h"

#include <iostream>
#include <vector>

int main(/*int argc, char** argv*/) {
    std::cout << "Barnes-Hut Sequential Implementation" << std::endl;

    int widthAndHeight = 800;

    // We generate the particles (10 thousand particles)
    std::vector<Particle*> particles = Particle::generateParticles(6, widthAndHeight, widthAndHeight);

    // We create a quadtree
    QuadTree qt;

    std::cout << "Inserting particles into the quadtree" << std::endl;

    // We insert the particles into the quadtree
    for (Particle* particle : particles) {
        std::cout << "Inserting particle at (" << particle->getX() << ", " << particle->getY() << ", " 
                    << particle->getMass() << ")" << std::endl;
        qt.insert(particle);
    }

    std::cout << "Particles inserted into the quadtree" << std::endl;

    // We print the quadtree structure in a window
    createWindow(&qt, widthAndHeight);

    // We do the simulation
    // TODO: Implement the simulation

    qt.print();

    waitClosedWindow();

    // We clear the quadtree
    qt.clear();

    // We clear the particles
    for (Particle* particle : particles) {
        delete particle;
    }

    std::cout << "End of Barnes-Hut Sequential Implementation" << std::endl;

    return 0;
}