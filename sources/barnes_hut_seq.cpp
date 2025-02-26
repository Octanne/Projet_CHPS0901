#include "barnes_hut_seq.h"

#include "quadtree.h"
#include "visualizer.h"

#include <iostream>
#include <vector>
#include <unistd.h>

int main(/*int argc, char** argv*/) {
    std::cout << "Barnes-Hut Sequential Implementation" << std::endl;

    int widthAndHeight = 1000;

    // We generate the particles (10 thousand particles)
    std::vector<Particle*> particles = Particle::generateParticles(100, widthAndHeight, widthAndHeight);

    // We create a quadtree
    QuadTree qt(widthAndHeight, widthAndHeight/2, widthAndHeight/2);

    std::cout << "Inserting particles into the quadtree" << std::endl;
    // We insert the particles into the quadtree
    for (Particle* particle : particles) {
        //std::cout << "BEGIN | Inserting particle at (" << particle->getX() << ", " << particle->getY() << ", " 
                    //<< particle->getMass() << ")" << std::endl;
        qt.insert(particle);
        //std::cout << "END | Inserting particle at (" << particle->getX() << ", " << particle->getY() << ", " 
                    //<< particle->getMass() << ")" << std::endl;
    }
    std::cout << "Particles inserted into the quadtree" << std::endl;

    // We print the quadtree structure in a window
    createWindow(&qt, widthAndHeight);

    // We do the simulation
    // TODO: Implement the simulation

    waitClosedWindow();

    // We wait the user to press a key
    std::cout << "Press a key to clear the quadtree and close the program" << std::endl;
    std::cin.get();

    // We clear the quadtree
    qt.clear();

    // We clear the particles
    for (Particle* particle : particles) {
        delete particle;
    }

    std::cout << "End of Barnes-Hut Sequential Implementation" << std::endl;

    return 0;
}