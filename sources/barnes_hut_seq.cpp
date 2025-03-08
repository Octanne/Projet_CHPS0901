#include "barnes_hut_seq.h"

#include <unistd.h>
#include <csignal>

#include <mpi.h>

// Defining the masses as constants
const double massEarth = 5.972e24; // in kilograms
const double massMoon = 7.347e22;  // in kilograms
const double massMars = 6.417e23;  // in kilograms
// Defining the masses of asteroids as constants
const double massCeres = 9.39e20;  // in kilograms
const double massVesta = 2.59e20;  // in kilograms
const double massEros = 6.69e15;   // in kilograms
const double massItokawa = 4.2e10; // in kilograms

// MPI variables
int rankMPI, sizeMPI;

int main(int argc, char** argv) {
    // Open MPI initialization
    MPI_Init(&argc, &argv);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rankMPI);
    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &sizeMPI);

    // We bind the MPI rank and size to the quadtree class
    QuadTree::setupMPIValues(&rankMPI, &sizeMPI);

    if (rankMPI == 0) std::cout << "Barnes-Hut MPI/OpenMP Implementation" << std::endl;

    double windowSizeG = 800; // default window size
    int numParticles = 1000; // default number of particles
    double maxMass = massEarth; // default max mass
    double minMass = massItokawa; // default min mass
    bool shouldGUI = false; // default no GUI
    double timeStep = 0.5; // default time step 500ms by default
    double refreshRate = 0.5; // default refresh rate 500ms by default
    double nbSteps = 0; // default number of steps
    double debugMode = false; // default no debug mode
    std::string filename; // default no file
    std::vector<Particle*> particles; // vector of particles

    // We handle the program options
    if (handleProgramOptions(argc, argv, nbSteps, refreshRate, debugMode, timeStep, filename, windowSizeG, 
            numParticles, maxMass, minMass, shouldGUI)) {
        MPI_Finalize();
        return 0;
    }
    
    // We load/generated the particles
    loadParticles(particles, filename, windowSizeG, numParticles, maxMass, minMass);

    // We init the default quadtree
    QuadTree qt = initQuadTree(particles, windowSizeG);

    // We create the quadtree visualizer
    Visualizer *qtVisu = Visualizer::getInstance(&qt, 800);
    initVisualizer(qtVisu, shouldGUI, debugMode);

    // We print the simulation parameters
    if (rankMPI == 0) {
        if (nbSteps != 0) std::cout << "Running simulation for " << nbSteps << " steps with a time step of " 
            << timeStep << "s" << std::endl;
        else std::cout << "Running simulation indefinitely with a time step of " 
            << timeStep << "s" << std::endl;
    }

    // We do the simulation
    double startTime = MPI_Wtime();
    launchSimulation(qt, particles, qtVisu, nbSteps, timeStep, refreshRate, shouldGUI);
    double endTime = MPI_Wtime();
    if (rankMPI == 0) {
        std::cout << "Simulation time: " << (endTime - startTime) << " seconds" << std::endl;
    }

    // We clean after the simulation
    cleanVisualizer(qtVisu, shouldGUI);

    // We clear the quadtree
    qt.clear();

    // We clean the particles and save them if needed
    cleanParticles(particles, true);

    if (rankMPI == 0) std::cout << "End of Simulation with Barnes-Hut MPI/OpenMP Implementation" << std::endl;

    // Open MPI finalization
    MPI_Finalize();

    return 0;
}

void launchSimulation(QuadTree &qt, std::vector<Particle *> &particles, Visualizer *qtVisu, double nbSteps, double timeStep, double refreshRate, bool shouldGUI)
{
    int step = 0;
    if (rankMPI == 0) std::cout << "Starting simulation with " << particles.size() << " particles" << std::endl;
    // We do the simulation
    while (!qtVisu->hasToClose() && (nbSteps == 0 || step < nbSteps))
    {
        // We update the position of the particles
        if (!qtVisu->isInPause())
        {
            if (qtVisu->isInDebug() && rankMPI == 0) std::cout << "Updating particles" << std::endl;
            qt.updateParticles(timeStep);
            if (qtVisu->isInDebug() && rankMPI == 0) std::cout << "Particles updated" << std::endl;
            // We update the tree
            if (qtVisu->isInDebug() && rankMPI == 0) std::cout << "Updating quadtree" << std::endl;
            qt.buildTree();
            if (qtVisu->isInDebug() && rankMPI == 0) std::cout << "Quadtree updated" << std::endl;
            step++;
        }
        // Refresh every refreshRate if GUI is enabled
        if (shouldGUI) usleep(refreshRate * 1000000);
        // We print the quadtree
        if (!shouldGUI && qtVisu->isInDebug()) qt.print();
    }
}

QuadTree initQuadTree(std::vector<Particle *> &particles, double windowSizeG)
{
    QuadTree::setDebugModePtr(Visualizer::ptrDebugMode());
    QuadTree qt(windowSizeG, 0, 0, &particles);
    qt.buildTree();
    if (rankMPI == 0) std::cout << "The simulation window size is " << qt.getWidth() << std::endl;
    return qt;
}

void cleanVisualizer(Visualizer *qtVisu, bool& shouldGUI)
{
    if (shouldGUI && rankMPI == 0)
    {
        std::cout << "End of simulation : waiting for the window to be closed" << std::endl;
        //qtVisu->closeWindow();
        qtVisu->waitClosedWindow();
    }
}

void initVisualizer(Visualizer *qtVisu, bool shouldGUI, double debugMode)
{
    qtVisu->setDebug(debugMode);
    if (shouldGUI && rankMPI == 0)
    {
        qtVisu->createWindow();
        qtVisu->setPause(true);
    }
    else if (rankMPI == 0)
    {
        // We put the signal handler to close the program
        signal(SIGINT, [](int signum)
                {
        Visualizer::getInstance()->closeWindow();
        printf("Interrupt signal received (%d)\n", signum); });
    }
}

void cleanParticles(std::vector<Particle *> &particles, bool saveParticles)
{
    // We save the particles to a file particle_output_date.txt
    if (saveParticles && rankMPI == 0) {
        std::string filenameOutput = "results/particle_output_" + std::to_string(time(0)) + ".txt";
        Particle::saveParticles(filenameOutput, particles);
        std::cout << "Particles status result saved to " << filenameOutput << std::endl;
    }
    // We clear the particles
    for (Particle *particle : particles)
    {
        delete particle;
    }
}

void loadParticles(std::vector<Particle *> &particles, std::string &filename, double &windowSizeG, int &numParticles, double &maxMass, double &minMass)
{
    if (!filename.empty())
    {
        if (rankMPI == 0) std::cout << "Loading particles from file " << filename << std::endl;
        particles = Particle::loadParticles(filename);
        // We print the particles loaded
        if (rankMPI == 0) {
            for (Particle *particle : particles)
            {
                std::cout << "Particle at (" << particle->getX() << ", " << particle->getY() << ") with mass " << particle->getMass() << std::endl;
            }
        }
    }
    else
    {
        // make it only done by rank 0 and then broadcast the particles to the other ranks
        if (rankMPI == 0) {
            std::cout << "Generating " << numParticles << " particles" << std::endl;
            // We generate the particles
            particles = Particle::generateParticles(numParticles, windowSizeG, windowSizeG, maxMass, minMass);
            // We send the particles list to all the other ranks
            int particleDataSize = numParticles * 5; // Each particle has 5 attributes: x, y, v_x, v_y, mass
            double* particleData = new double[particleDataSize];
            for (int i = 0; i < numParticles; i++) {
                particleData[i * 5] = particles[i]->getX();
                particleData[i * 5 + 1] = particles[i]->getY();
                particleData[i * 5 + 2] = particles[i]->getVx();
                particleData[i * 5 + 3] = particles[i]->getVy();
                particleData[i * 5 + 4] = particles[i]->getMass();
            }
            for (int i = 1; i < sizeMPI; i++) {
                std::cout << "Sending particles to rank " << i << std::endl;
                MPI_Send(particleData, particleDataSize, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
                std::cout << "Particles sent to rank " << i << std::endl;
            }
            delete[] particleData;
        } else {
            int particleDataSize = numParticles * 5;
            double* particleData = new double[particleDataSize];
            MPI_Recv(particleData, particleDataSize, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int i = 0; i < numParticles; i++) {
                double x = particleData[i * 5];
                double y = particleData[i * 5 + 1];
                double v_x = particleData[i * 5 + 2];
                double v_y = particleData[i * 5 + 3];
                double mass = particleData[i * 5 + 4];
                Particle *particle = new Particle(x, y, mass);
                particle->setVx(v_x);
                particle->setVy(v_y);
                particles.push_back(particle);
            }
            delete[] particleData;
        }
        std::cout << "Particles received by rank " << rankMPI << std::endl;
    }
}

int handleProgramOptions(int argc, char **argv, double &nbSteps, double &refreshRate, double &debugMode, double &timeStep, std::string &filename, double &windowSizeG, int &numParticles, double &maxMass, double &minMass, bool &shouldGUI)
{
    int opt;
    bool wFlag = true;
    bool nFlag = false;
    while ((opt = getopt(argc, argv, "w:n:G:L:ghf:t:de:r:")) != -1)
    {
        switch (opt)
        {
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
            return 1;
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
            if (rankMPI == 0) std::cerr << "Usage: " << argv[0] << " -w <window_size> -n <num_particles> [ -G <max_mass> -L <min_mass> -g -f <file>]" << std::endl;
            return 1;
        }
    }

    if (filename.empty() && (!wFlag || !nFlag))
    {
        if (rankMPI == 0) {
            std::cerr << "Error: -w <window_size> and -n <num_particles> are mandatory unless -f <file> is used." << std::endl;
            std::cerr << "Usage: " << argv[0] << " -w <window_size> -n <num_particles> [ -G <max_mass> -L <min_mass> -g -f <file>]" << std::endl;
        }
        return 1;
    }
    return 0;
}

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
