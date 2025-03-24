
#include "quadtree.h"
#include "visualizer.h"

#include <iostream>
#include <vector>

/**
 * @brief Retrieves an integer value from the command line option argument.
 *
 * This function processes the command line arguments to extract an integer value.
 *
 * @return int The extracted integer value.
 */
int getAnIntFromOptarg();

/**
 * @brief Retrieves a double value from the command line option argument.
 *
 * This function processes the command line arguments to extract a double value.
 *
 * @return double The extracted double value.
 */
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

/**
 * @brief Launches the simulation.
 *
 * This function runs the simulation using the provided quadtree, particles, visualizer,
 * number of steps, time step, refresh rate, debug mode, and GUI flag.
 *
 * @param qt Reference to the quadtree.
 * @param particles Vector of particle pointers.
 * @param qtVisu Pointer to the visualizer.
 * @param nbSteps Number of steps for the simulation.
 * @param timeStep Time step for the simulation.
 * @param refreshRate Refresh rate of the simulation.
 * @param debugMode Debug mode flag.
 * @param shouldGUI Flag to enable or disable GUI.
 */
void launchSimulation(QuadTree &qt, std::vector<Particle *> &particles, Visualizer *qtVisu, double nbSteps, double timeStep, double refreshRate, bool shouldGUI);

/**
 * @brief Handles the program options from the command line arguments.
 *
 * This function processes the command line arguments to set the simulation parameters.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @param nbSteps Reference to the number of steps for the simulation.
 * @param refreshRate Reference to the refresh rate of the simulation.
 * @param debugMode Reference to the debug mode flag.
 * @param timeStep Reference to the time step for the simulation.
 * @param filename Reference to the filename for loading particles.
 * @param windowSizeG Reference to the window size.
 * @param numParticles Reference to the number of particles.
 * @param maxMass Reference to the maximum mass of the particles.
 * @param minMass Reference to the minimum mass of the particles.
 * @param shouldGUI Reference to the flag to enable or disable GUI.
 * @param saveParticles Reference to the flag to save the particle state.
 * @return int Status of the option handling.
 */
int handleProgramOptions(int argc, char **argv, double &nbSteps, double &refreshRate, double &debugMode, double &timeStep, std::string &filename, double &windowSizeG, int &numParticles, double &maxMass, double &minMass, bool &shouldGUI, bool &saveParticles);

/**
 * @brief Loads particles from a file.
 *
 * This function loads particles from the specified file and initializes their properties.
 *
 * @param particles Vector of particle pointers.
 * @param filename Reference to the filename for loading particles.
 * @param windowSizeG Reference to the window size.
 * @param numParticles Reference to the number of particles.
 * @param maxMass Reference to the maximum mass of the particles.
 * @param minMass Reference to the minimum mass of the particles.
 */
void loadParticles(std::vector<Particle *> &particles, std::string &filename, double &windowSizeG, int &numParticles, double &maxMass, double &minMass);

/**
 * @brief Cleans up the particles.
 *
 * This function cleans up the particles, optionally saving their state.
 *
 * @param particles Vector of particle pointers.
 * @param save Flag to indicate whether to save the particle state.
 */
void cleanParticles(std::vector<Particle *> &particles, bool save);

#if VISU
/**
 * @brief Cleans up the visualizer.
 *
 * This function cleans up the visualizer, optionally based on the GUI flag.
 *
 * @param qtVisu Pointer to the visualizer.
 * @param shouldGUI Reference to the flag to enable or disable GUI.
 */
void cleanVisualizer(Visualizer *qtVisu, bool& shouldGUI);

/**
 * @brief Initializes the visualizer.
 *
 * This function initializes the visualizer based on the GUI and debug mode flags.
 *
 * @param qtVisu Pointer to the visualizer.
 * @param shouldGUI Flag to enable or disable GUI.
 * @param debugMode Debug mode flag.
 */
void initVisualizer(Visualizer *qtVisu, bool shouldGUI, double debugMode);
#endif
/**
 * @brief Initializes the quadtree.
 *
 * This function initializes the quadtree based on the particles and window size.
 *
 * @param particles Vector of particle pointers.
 * @param windowSizeG Reference to the window size.
 * @return QuadTree The initialized quadtree.
 */
QuadTree initQuadTree(std::vector<Particle *> &particles, double windowSizeG);