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

/**
 * @brief Main function for the Barnes-Hut Sequential Implementation.
 *
 * This function initializes the simulation parameters from command line arguments,
 * generates particles, builds a quadtree, and runs the simulation.
 *
 * Command line arguments:
 * -w <window_size> : Set the width/height of the simulation window (squared) (mandatory).
 * -n <num_particles> : Set the number of particles (mandatory).
 * -G <max_mass> : Set the maximum mass of the particles (optional, default is massEarth).
 * -L <min_mass> : Set the minimum mass of the particles (optional, default is massItokawa).
 * -g : Enable GUI (optional).
 * -h : Print this help message.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return int Exit status of the program.
 */
int main(int argc, char** argv) {
    std::cout << "Barnes-Hut Sequential Implementation" << std::endl;

    // We get arguments from the command line to set :
    // the width/height of the simulation window (squared)
    // the number of particles
    // the maximum mass of the particles
    // the minimum mass of the particles

    int opt;
    double windowSize = 800; // default window size
    int numParticles = 1000; // default number of particles
    double maxMass = massEarth; // default max mass
    double minMass = massItokawa; // default min mass
    bool shouldGUI = false; // default no GUI

    bool wFlag = false, nFlag = false;

    while ((opt = getopt(argc, argv, "w:n:G:L:gh")) != -1) {
        switch (opt) {
            case 'w':
                windowSize = std::stod(optarg);
                wFlag = true;
                break;
            case 'n':
                numParticles = std::stoi(optarg);
                nFlag = true;
                break;
            case 'G':
                maxMass = std::stod(optarg);
                break;
            case 'L':
                minMass = std::stod(optarg);
                break;
            case 'g':
                shouldGUI = true;
                break;
            case 'h':
                std::cout << "Command line arguments:\n"
                          << " * -w <window_size> : Set the width/height of the simulation window (squared) (mandatory).\n"
                          << " * -n <num_particles> : Set the number of particles (mandatory).\n"
                          << " * -G <max_mass> : Set the maximum mass of the particles (optional, default is massEarth).\n"
                          << " * -L <min_mass> : Set the minimum mass of the particles (optional, default is massItokawa).\n"
                          << " * -g : Enable GUI (optional).\n"
                          << " * -h : Print this help message.\n";
                return 0;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " -w <window_size> -n <num_particles> [ -G <max_mass> -L <min_mass> -g]" << std::endl;
                return 1;
        }
    }

    if (!wFlag || !nFlag) {
        std::cerr << "Error: -w <window_size> and -n <num_particles> are mandatory." << std::endl;
        std::cerr << "Usage: " << argv[0] << " -w <window_size> -n <num_particles> [ -G <max_mass> -L <min_mass> -g]" << std::endl;
        return 1;
    }

    // We generate the particles
    std::vector<Particle*> particles = Particle::generateParticles(numParticles, windowSize, windowSize, maxMass, minMass);

    // We create a quadtree
    QuadTree qt(windowSize, windowSize/2, windowSize/2, &particles);

    std::cout << "Inserting particles into the quadtree" << std::endl;
    qt.buildTree();
    std::cout << "Particles inserted into the quadtree" << std::endl;

    // We print the quadtree structure in a window
    if (shouldGUI) {
        createWindow(&qt, 800, windowSize);
    } else {
        // We put the signal handler to close the program
        signal(SIGINT, [](int signum) {
            shouldClose = true;
            printf("Interrupt signal received (%d)\n", signum);
        });
    }

    // We do the simulation
    while (!shouldClose) {
        // We update the position of the particles
        if (!shouldPause) {
            std::cout << "Updating particles" << std::endl;
            qt.updateParticles(0.5);
            std::cout << "Particles updated" << std::endl;
            // We update the tree
            std::cout << "Updating quadtree" << std::endl;
            qt.buildTree();
            std::cout << "Quadtree updated" << std::endl;
        }
        usleep(500000); // We sleep for 500ms
        if (!shouldGUI) qt.print();
    }

    if (shouldGUI) {
        std::cout << "End of simulation : waiting for the window to be closed" << std::endl;
        waitClosedWindow();
    }

    // We clear the quadtree
    qt.clear();

    // We clear the particles
    for (Particle* particle : particles) {
        delete particle;
    }

    std::cout << "End of Simulation with Barnes-Hut Sequential Implementation" << std::endl;

    return 0;
}