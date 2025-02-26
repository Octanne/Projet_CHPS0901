#include "barnes_hut_seq.h"

#include "quadtree.h"
#include "visualizer.h"

#include <iostream>
#include <vector>
#include <unistd.h>
#include <csignal>

// Defining the masses as constants
const double massEarth = 5.972e24; // in kilograms
const double massMoon = 7.347e22;  // in kilograms
const double massMars = 6.417e23;  // in kilograms
// Defining the masses of asteroids as constants
const double massCeres = 9.39e20;  // in kilograms
const double massVesta = 2.59e20;  // in kilograms
const double massEros = 6.69e15;   // in kilograms
const double massItokawa = 4.2e10; // in kilograms

int main(/*int argc, char** argv*/) {
    std::cout << "Barnes-Hut Sequential Implementation" << std::endl;

    double widthAndHeight = 1000000;

    // We generate the particles (10 thousand particles)
    std::vector<Particle*> particles = Particle::generateParticles(10, widthAndHeight, widthAndHeight, massEarth, massItokawa);

    // We create a quadtree
    QuadTree qt(widthAndHeight, widthAndHeight/2, widthAndHeight/2);

    std::cout << "Inserting particles into the quadtree" << std::endl;
    // We insert the particles into the quadtree
    for (Particle* particle : particles) {
        qt.insert(particle);
    }
    std::cout << "Particles inserted into the quadtree" << std::endl;

    // We print the quadtree structure in the console
    //qt.print();

    // We print the quadtree structure in a window
    createWindow(&qt, 10000, 1000);

    // We do the simulation
    while (!shouldClose) {
        for (int i = 0; i < 60 && !shouldClose; i++) {
            // We update the position of the particles
            std::cout << "Updating particles" << std::endl;
            qt.updateVelocities(1);
            std::cout << "Particles updated" << std::endl;
            // We update the tree
            std::cout << "Updating quadtree" << std::endl;
            qt.clear();
            for (Particle* particle : particles) {
                qt.insert(particle);
            }
            std::cout << "Quadtree updated" << std::endl;
            usleep(1000000); // We sleep for 1000ms
        }
        // We ask if we want to go to n iterations or not
        if (shouldClose) break;
        std::cout << "Do you want to do 60 more iterations? (y/n)" << std::endl;
        char answer;
        std::cin >> answer;
        if (answer == 'y') continue;
        else break;
    }

    std::cout << "End of simulation : waiting for the window to be closed" << std::endl;
    //waitClosedWindow();

    // We clear the quadtree
    qt.clear();

    // We clear the particles
    for (Particle* particle : particles) {
        delete particle;
    }

    std::cout << "End of Barnes-Hut Sequential Implementation" << std::endl;

    return 0;
}