#ifndef BARNES_HUT_SEQ_H
#define BARNES_HUT_SEQ_H

// Your code here
int getAnIntFromOptarg();
double getADoubleFromOptarg();

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
int main(int argc, char **argv);

int handleProgramOptions(int argc, char **argv, double &nbSteps, double &refreshRate, double &debugMode, double &timeStep, std::string &filename, double &windowSizeG, int &numParticles, double &maxMass, double &minMass, bool &shouldGUI);
void loadParticles(std::vector<Particle *> &particles, std::string &filename, double &windowSizeG, int &numParticles, double &maxMass, double &minMass);

#endif // BARNES_HUT_SEQ_H