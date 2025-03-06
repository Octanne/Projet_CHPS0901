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

double getADoubleFromOptarg() {
    // Check if not null
    if (optarg == nullptr) {
        std::cerr << "Error: The value must be a number." << std::endl;
        exit(1);
    }
    // Convert the optarg to a double check if stod failed
    double value;
    try {
        value = std::stod(optarg);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid argument for conversion to double." << std::endl;
        exit(1);
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Argument out of range for conversion to double." << std::endl;
        exit(1);
    }
    // Check if the value is positive
    if (value <= 0) {
        std::cerr << "Error: The value must be positive." << std::endl;
        exit(1);
    }
    return value;
}

int getAnIntFromOptarg() {
    // Check if not null
    if (optarg == nullptr) {
        std::cerr << "Error: The value must be a number." << std::endl;
        exit(1);
    }
    // Convert the optarg to an int check if stoi failed
    int value;
    try {
        value = std::stoi(optarg);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid argument for conversion to int." << std::endl;
        exit(1);
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Argument out of range for conversion to int." << std::endl;
        exit(1);
    }
    // Check if the value is positive
    if (value <= 0) {
        std::cerr << "Error: The value must be positive." << std::endl;
        exit(1);
    }
    return value;
}

/**
 * @brief Main function for the Barnes-Hut Sequential Implementation.
 *
 * This function initializes the simulation parameters from command line arguments,
 * generates particles, builds a quadtree, and runs the simulation.
 *
 * Command line arguments:
 * -w <window_size> : Set the width/height of the generate window (squared) (mandatory).
 * -n <num_particles> : Set the number of particles (mandatory).
 * -G <max_mass> : Set the maximum mass of the particles (optional, default is massEarth).
 * -L <min_mass> : Set the minimum mass of the particles (optional, default is massItokawa).
 * -e <num_steps> : Set the number of steps for the simulation (optional, default is none).
 * -g : Enable GUI (optional).
 * -f <file> : Load particles from a file (optional).
 * -t <step> : Set the time step for the simulation (optional, default is 0.5s).
 * -r <refresh_rate> : Set the refresh rate of the simulation (optional, default is 0.5s).
 * -d : Enable debug mode (optional).
 * -h : Print this help message.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return int Exit status of the program.
 */
int main(int argc, char** argv) {
    std::cout << "Barnes-Hut Sequential Implementation" << std::endl;

    int opt;
    double windowSizeG = 800; // default window size
    int numParticles = 1000; // default number of particles
    double maxMass = massEarth; // default max mass
    double minMass = massItokawa; // default min mass
    bool shouldGUI = false; // default no GUI
    std::vector<Particle*> particles;
    bool wFlag = true, nFlag = false;
    double timeStep = 0.5; // default time step 500ms by default
    double refreshRate = 0.5; // default refresh rate 500ms by default
    double nbSteps = 0; // default number of steps
    double debugMode = false;

    std::string filename;
    while ((opt = getopt(argc, argv, "w:n:G:L:ghf:t:de:r:")) != -1) {
        switch (opt) {
            case 'h':
                std::cout << "Command line arguments:\n"
                          << " * -w <window_size> : Set the width/height of the generate window (squared) (mandatory unless -f is used).\n"
                          << " * -n <num_particles> : Set the number of particles (mandatory unless -f is used).\n"
                          << " * -G <max_mass> : Set the maximum mass of the particles (optional, default is massEarth).\n"
                          << " * -L <min_mass> : Set the minimum mass of the particles (optional, default is massItokawa).\n"
                          << " * -g : Enable GUI (optional).\n"
                          << " * -f <file> : Load particles from a file (optional).\n"
                          << " * -e <num_steps> : Set the number of steps for the simulation (optional, default is none).\n"
                          << " * -t <time_step> : Set the time step for the simulation (optional, default is 0.5).\n"
                          << " * -r <refresh_rate> : Set the refresh rate of the simulation (optional, default is 0.5).\n"
                          << " * -d : Enable debug mode (optional).\n"
                          << " * -h : Print this help message.\n";
                return 0;
                break;
            case 'e':
                nbSteps = getADoubleFromOptarg();
                break;
            case 'r': 
                refreshRate = getADoubleFromOptarg();
                break;
            case 'd':
                debugMode = true;
                break;
            case 't':
                timeStep = getADoubleFromOptarg();
                break;
            case 'f':
                filename = optarg;
                break;
            case 'w':
                windowSizeG = getADoubleFromOptarg();
                wFlag = true;
                break;
            case 'n':
                numParticles = getAnIntFromOptarg();
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
            default:
                std::cerr << "Usage: " << argv[0] << " -w <window_size> -n <num_particles> [ -G <max_mass> -L <min_mass> -g -f <file>]" << std::endl;
                return 1;
        }
    }

    if (filename.empty() && (!wFlag || !nFlag)) {
        std::cerr << "Error: -w <window_size> and -n <num_particles> are mandatory unless -f <file> is used." << std::endl;
        std::cerr << "Usage: " << argv[0] << " -w <window_size> -n <num_particles> [ -G <max_mass> -L <min_mass> -g -f <file>]" << std::endl;
        return 1;
    }

    if (!filename.empty()) {
        std::cout << "Loading particles from file " << filename << std::endl;
        particles = Particle::loadParticles(filename);
        // We print the particles loaded
        for (Particle* particle : particles) {
            std::cout << "Particle at (" << particle->getX() << ", " << particle->getY() << ") with mass " << particle->getMass() << std::endl;
        }
    } else {
        // We generate the particles
        particles = Particle::generateParticles(numParticles, windowSizeG, windowSizeG, maxMass, minMass);
    }
    
    // We create a quadtree
    QuadTree::setDebugModePtr(Visualizer::ptrDebugMode());
    QuadTree qt(windowSizeG, windowSizeG/2, windowSizeG/2, &particles);
    qt.buildTree();
    std::cout << "The simulation window size is " << qt.getWidth() << std::endl;

    // We print the quadtree structure in a window
    Visualizer* qtVisu = Visualizer::getInstance(&qt, 800);
    qtVisu->setDebug(debugMode);
    if (shouldGUI) {
        qtVisu->createWindow();
        qtVisu->setPause(true);
    } else {
        // We put the signal handler to close the program
        signal(SIGINT, [](int signum) {
            Visualizer::getInstance()->closeWindow();
            printf("Interrupt signal received (%d)\n", signum);
        });
    }

    if (nbSteps != 0) {
        std::cout << "Running simulation for " << nbSteps << " steps" << std::endl;
    } else {
        std::cout << "Running simulation indefinitely" << std::endl;
    }

    // We do the simulation
    int step = 0;
    while (!qtVisu->hasToClose() && (nbSteps == 0 || step < nbSteps)) {
        // We update the position of the particles
        if (!qtVisu->isInPause()) {
            if (qtVisu->isInDebug()) std::cout << "Updating particles" << std::endl;
            qt.updateParticles(timeStep);
            if (qtVisu->isInDebug()) std::cout << "Particles updated" << std::endl;
            // We update the tree
            if (qtVisu->isInDebug()) std::cout << "Updating quadtree" << std::endl;
            qt.buildTree();
            if (qtVisu->isInDebug()) std::cout << "Quadtree updated" << std::endl;
            step++;
        }
        // Refresh every refreshRate
        usleep(refreshRate * 1000000);
        // We print the quadtree
        if (!shouldGUI && qtVisu->isInDebug()) qt.print();
    }

    if (shouldGUI) {
        std::cout << "End of simulation : waiting for the window to be closed" << std::endl;
        qtVisu->closeWindow();
        qtVisu->waitClosedWindow();
    }

    // We clear the quadtree
    qt.clear();

    // We save the particles to a file particle_output_date.txt
    std::string filenameOutput = "results/particle_output_" + std::to_string(time(0)) + ".txt";
    Particle::saveParticles(filenameOutput, particles);
    std::cout << "Particles status result saved to " << filenameOutput << std::endl;

    // We clear the particles
    for (Particle* particle : particles) {
        delete particle;
    }

    std::cout << "End of Simulation with Barnes-Hut Sequential Implementation" << std::endl;

    return 0;
}