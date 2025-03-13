#include "quadtree.h"
#include "particle.h"

#include <iostream>
#include <cmath>
#include <queue>
#include <stack>
#include <algorithm>

#include <mpi.h>
#include <omp.h>

bool* QuadTree::debugModePtr = nullptr;
int* QuadTree::rankMPI = nullptr;
int* QuadTree::sizeMPI = nullptr;
std::vector<QuadTree*> QuadTree::nodePool;

bool QuadTree::debugMode() {
    return *debugModePtr;
}

void QuadTree::setDebugModePtr(bool* debugMode) {
    debugModePtr = debugMode;
}

QuadTree::QuadTree(double width, double originX, double originY, std::vector<Particle*>* particles)
    : particle(nullptr), isDivided(false), hasBody(false), width(width), weightBranch(0),
      originX(originX), originY(originY), northeast(nullptr), northwest(nullptr),
      southeast(nullptr), southwest(nullptr), particles(particles) {}

QuadTree::QuadTree(double width, double originX, double originY, int weightBranch)
    : particle(nullptr), isDivided(false), hasBody(false), width(width), weightBranch(weightBranch),
      originX(originX), originY(originY), northeast(nullptr), 
      northwest(nullptr), southeast(nullptr), southwest(nullptr) {
    this->particles = new std::vector<Particle*>();
}

std::string spacer(int depth) {
    std::string s = "";
    for (int i = 0; i < depth; i++) {
        s += "  ";
    }
    return s;
}

void QuadTree::print(int depth) {
    if (isDivided) {
        std::cout << spacer(depth) << "NorthEast: (depth : " << depth << ")" << std::endl;
        northeast->print(depth + 1);
        std::cout << spacer(depth) << "NorthWest: (depth : " << depth << ")" << std::endl;
        northwest->print(depth + 1);
        std::cout << spacer(depth) << "SouthEast: (depth : " << depth << ")" << std::endl;
        southeast->print(depth + 1);
        std::cout << spacer(depth) << "SouthWest: (depth : " << depth << ")" << std::endl;
        southwest->print(depth + 1);
    } else {
        if (particle == nullptr) {
            std::cout << spacer(depth) << "Empty leaf" << std::endl;
        } else {
            std::cout << spacer(depth) << "Leaf at (" << particle->getX() << ", " << particle->getY() << ") with mass " << particle->getMass() << std::endl;
        }
    }
}

void QuadTree::insert(Particle* particleInsert) {
    insertSimple(particleInsert);    
    // We insert the particle in the list of particles
    particles->push_back(particleInsert);
}

void QuadTree::insertSimple(Particle* particleInsert) {
     // If node x does not contain a body, put the new body b here.
     if (!isDivided) {
        if (!hasBody) {
            particle = particleInsert;
            hasBody = true;
            // We increment the weight of the branch
            weightBranch++;
            return;
        } else {
            // Subdivide the region further by creating four children
            // We moving body b update the center-of-mass and total mass of x.
            subdivide();
            // We inserting the body c in the appropriate quadrant
        }
    }

    // If node x is an internal node, update the center-of-mass and total mass of x.
    updateCenterOfMass(particleInsert);
    // Recursively insert the body b in the appropriate quadrant.
    if (particleInsert->getX() < getOriginX()) { // We check for west quadrants
        if (particleInsert->getY() < getOriginY()) {
            southwest->insertSimple(particleInsert);
            //std::cout << "SouthWest inserted particle at (" << particleInsert->getX() << ", " << particleInsert->getY() << ", " << particleInsert->getMass() << ")" << std::endl;
        } else {
            northwest->insertSimple(particleInsert);
            //std::cout << "NorthWest inserted particle at (" << particleInsert->getX() << ", " << particleInsert->getY() << ", " << particleInsert->getMass() << ")" << std::endl;
        }
    } else { // We check for east quadrants
        if (particleInsert->getY() < getOriginY()) {
            southeast->insertSimple(particleInsert);
            //std::cout << "SouthEast inserted particle at (" << particleInsert->getX() << ", " << particleInsert->getY() << ", " << particleInsert->getMass() << ")" << std::endl;
        } else {
            northeast->insertSimple(particleInsert);
            //std::cout << "NorthEast inserted particle at (" << particleInsert->getX() << ", " << particleInsert->getY() << ", " << particleInsert->getMass() << ")" << std::endl;
        }
    }

    // We increment the weight of the branch
    weightBranch++;
}

void QuadTree::subdivide() {
    // We create the four quadrants
    double newWidth = width / 2;
    double center = newWidth / 2;
    northwest = getNode(newWidth, originX - center, originY + center, 0);
    northeast = getNode(newWidth, originX + center, originY + center, 0);
    southwest = getNode(newWidth, originX - center, originY - center, 0);
    southeast = getNode(newWidth, originX + center, originY - center, 0);
    isDivided = true; // The node is set to divided
    hasBody = false; // The node is not a leaf anymore

    // Recursively insert the body b in the appropriate quadrant.
    Particle* particleToMove = particle;
    particle = new Particle();
    //std::cout << "Subdivided the quadtree origin at (" << originX << ", " << originY << ") with width " << width << std::endl;
    insertSimple(particleToMove);
    // We decrement the weight of the branch because we have moved the particle and it will be inserted again
    weightBranch--;
}

void QuadTree::updateCenterOfMass(Particle* particleInsert) {
    double totalMass = particle->getMass() + particleInsert->getMass();
    particle->setX((particle->getX() * particle->getMass() + particleInsert->getX() * particleInsert->getMass()) / totalMass);
    particle->setY((particle->getY() * particle->getMass() + particleInsert->getY() * particleInsert->getMass()) / totalMass);
    particle->setMass(totalMass);
    //std::cout << "Center of mass updated" << std::endl;
}

double QuadTree::distance(double x1, double y1, double x2, double y2) const {
    // Calculate the distance between two points 
    // We use the euclidian distance formula
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void calcForce(Particle* b, Particle* c, double& fx, double& fy) {
    // We calculate the force exerted by the current node on b and add this amount to b's net force
    // We use the formula F = G * m1 * m2 / d^2
    // We add the force to the net force of the particle.
    double dx = b->getX() - c->getX();
    double dy = b->getY() - c->getY();
    double distance = std::sqrt((dx * dx) + (dy * dy));
    double accel = (-1 * Particle::G * c->getMass()) / std::pow(distance,2.0);
    double forceVecX = dx / distance;
    double forceVecY = dy / distance;

    fx += accel * forceVecX;
    fy += accel * forceVecY;
}

void QuadTree::calculateForce(Particle* b, double& fx, double& fy) const {
    std::stack<const QuadTree*> stack;
    stack.push(this);

    while (!stack.empty()) {
        const QuadTree* node = stack.top();
        stack.pop();

        if (!node->isDivided) {
            if (!node->hasBody) continue; // If the node is empty, we skip
            if (b != node->particle) {
                // We call the function to calculate the force between the two particles and add it to the net force of the particle.
                calcForce(b, node->particle, fx, fy);
            }
        } else {
            // Calculate the ratio s / d, where s is the width of the region represented by the internal node, and d is the distance between the body and the node's center-of-mass.
            double s = node->width;
            double d = distance(b->getX(), b->getY(), node->particle->getX(), node->particle->getY());
            // If ratio > theta we perform recursively on each of the current node's children.
            if (s / d < theta) {
                // Treat the internal node as a single body, and calculate the force it exerts on body b, and add this amount to b's net force.
                calcForce(b, node->particle, fx, fy);
            } else {
                if (node->northeast != nullptr) stack.push(node->northeast);
                if (node->northwest != nullptr) stack.push(node->northwest);
                if (node->southeast != nullptr) stack.push(node->southeast);
                if (node->southwest != nullptr) stack.push(node->southwest);
            }
        }
    }
}

std::vector<std::vector<QuadTree*>> QuadTree::computeBalancedRanks(int nRank) const {
    std::vector<std::vector<QuadTree*>> result(nRank);
    if (nRank <= 0) return result;

    double totalWeight = static_cast<double>(this->weightBranch);
    double targetPerRank = totalWeight / nRank;
    int currentRank = 0;
    double currentSum = 0.0;

    //std::cout << "Total weight " << totalWeight << " and target per rank " << targetPerRank << std::endl;

    std::queue<const QuadTree*> bfsQueue;
    // On ajoute les enfants du noeud courant si présent dans la file car sinon le cas 1 mets fin à la boucle
    if (isDivided) {
        // On ajoute seulement les enfants > 0
        if (northeast->weightBranch > 0) bfsQueue.push(northeast);
        if (northwest->weightBranch > 0) bfsQueue.push(northwest);
        if (southeast->weightBranch > 0) bfsQueue.push(southeast);
        if (southwest->weightBranch > 0) bfsQueue.push(southwest);
    } else {
        bfsQueue.push(this);
    }

    while (!bfsQueue.empty() && currentRank < nRank) {
        const QuadTree* node = bfsQueue.front();
        bfsQueue.pop();

        // Cas 1 : Le noeud rentre dans le rang actuel
        if (currentSum + node->weightBranch <= targetPerRank || (bfsQueue.empty() && !node->isDivided)) {
            result[currentRank].push_back(const_cast<QuadTree*>(node));
            currentSum += node->weightBranch;
        }
        // Cas 2 : Le noeud est trop gros mais divisible -> on explore ses enfants
        else if (node->isDivided) {
            // On ajoute seulement les enfants > 0
            if (node->northeast->weightBranch > 0) bfsQueue.push(node->northeast);
            if (node->northwest->weightBranch > 0) bfsQueue.push(node->northwest);
            if (node->southeast->weightBranch > 0) bfsQueue.push(node->southeast);
            if (node->southwest->weightBranch > 0) bfsQueue.push(node->southwest);
        }
        // Cas 3 : Noeud indivisible trop gros -> on l'affecte malgré tout
        else {
            result[currentRank].push_back(const_cast<QuadTree*>(node));
            currentSum = targetPerRank; // Force passage au rang suivant
        }

        // Passage au rang suivant si quota atteint
        if (currentSum >= targetPerRank && currentRank < nRank - 1) {
            currentRank++;
            currentSum = 0.0;
        }
    }

    return result;
}

// TODO MAYBE DO A FUSION OF PARTICLES WHEN THEY COLLIDE TO MAKE THEM SPREAD
// in multiple particles but we keep the mass total to avoid losing matter (energy conservation)
// We can also do a fusion of particles when they are too close to each other
// We can also do a fusion of particles when they are too close to the center of mass TO AVOID 
// THE COLLAPSE OF THE SYSTEM (To much divided sectors)

void QuadTree::updateParticles(double step) { 
    // We compute the position of the start of the subtree to be handled by each rank
    std::vector<std::vector<QuadTree*>> poOfSubtree = computeBalancedRanks(*sizeMPI);

    // Stockage des accélérations locales (chaque rang calcule sa contribution)
    //std::vector<double> localAccX(particles->size(), 0.0);
    //std::vector<double> localAccY(particles->size(), 0.0);
    double localAccX[particles->size()];
    double localAccY[particles->size()];
    #pragma omp parallel for
    for (size_t i = 0; i < particles->size(); ++i) {
        localAccX[i] = 0.0;
        localAccY[i] = 0.0;
    }

    // We get are nodes list to handle
    std::vector<QuadTree*> nodesToHandle = poOfSubtree[*rankMPI];
    // We handle the nodes
    
    // We compute the forces exerted on the particles
    // CAN BE OPTIMIZED BY OPENMP
    #pragma omp parallel for reduction(+:localAccX[:particles->size()]) reduction(+:localAccY[:particles->size()])
    for (size_t i = 0; i < particles->size(); ++i) {
        Particle* particle = (*particles)[i];
        for (QuadTree* node : poOfSubtree[*rankMPI]) {
            double fx = 0.0, fy = 0.0;
            node->calculateForce(particle, fx, fy); // Déjà basé sur G*mass/d²
            localAccX[i] += fx; // fx contient l'accélération (pas besoin de diviser par mass)
            localAccY[i] += fy;
        }
    }
    
    // MPI AllReduce pour sommer les accélérations locales
    //MPI_Allreduce(MPI_IN_PLACE, localAccX.data(), particles->size(), MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    //MPI_Allreduce(MPI_IN_PLACE, localAccY.data(), particles->size(), MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(MPI_IN_PLACE, localAccX, particles->size(), MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(MPI_IN_PLACE, localAccY, particles->size(), MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    // On met à jour les vitesses et positions des particules
    // CAN BE OPTIMIZED BY OPENMP
    #pragma omp parallel for
    for (size_t i = 0; i < particles->size(); ++i) {
        Particle* particle = (*particles)[i];

        // We print the force exerted on the particle
        //if (debugMode()) std::cout << "Particle at (" << particle->getX() << ", " << particle->getY() 
        //    << ") has force (" << localAccX[i] << ", " << localAccY[i] << ")" << std::endl;

        // We update the velocity of the particle
        particle->setVx(particle->getVx() + localAccX[i] * step);
        particle->setVy(particle->getVy() + localAccY[i] * step);

        // We print the velocity of the particle
        //if (debugMode()) std::cout << "Particle at (" << particle->getX() << ", " << particle->getY()
        //     << ") has velocity (" << particle->getVx() << ", " << particle->getVy() << ")" << std::endl;

        // We update the position of the particle
        particle->setX(particle->getX() + particle->getVx() * step);
        particle->setY(particle->getY() + particle->getVy() * step);

        // We print the position of the particle
        //if (debugMode()) std::cout << "Particle now at (" << particle->getX() << ", " << particle->getY() 
        //    << ")" << std::endl;
    }
}

QuadTree::~QuadTree() {
    clear();
}

QuadTree* QuadTree::getNode(double w, double ox, double oy, int weightBranch) {
    if (nodePool.empty()) {
        return new QuadTree(w, ox, oy, 0);
    }
    QuadTree* node = nodePool.back();
    nodePool.pop_back();
    node->reset(); // Clear children/particle
    node->width = w; node->originX = ox; node->originY = oy; node->weightBranch = weightBranch;
    return node;
}

void QuadTree::recycle() {
    if (isDivided) {
        northeast->recycle();
        northwest->recycle();
        southeast->recycle();
        southwest->recycle();
    }
    nodePool.push_back(this);
}

void QuadTree::reset() {
    if (particle != nullptr && isDivided) {
        delete particle;

        northeast->reset();
        northwest->reset();
        southeast->reset();
        southwest->reset();
    }
    particle = nullptr;

    isDivided = false;
    hasBody = false;
    weightBranch = 0;
    width = 0;
    originX = 0;
    originY = 0;

    // We set the children to nullptr
    northeast = nullptr;
    northwest = nullptr;
    southeast = nullptr;
    southwest = nullptr;
    // particles = nullptr; // We do not delete the particles listes
}

void QuadTree::clear() {
    if (isDivided) {
        //std::cerr << "Deleting quadtree" << std::endl;
        //std::cerr << "Deleting the quadtree at (" << originX << ", " << originY << ")" << std::endl;
        // We delete the particle if divided because it as been created by the quadtree
        if (particle != nullptr) {
            delete particle;
            //std::cerr << "Delete center of mass at (" << originX << ", " << originY << ") done" << std::endl;
        }/* else {
            std::cerr << "No center of mass to delete at (" << originX << ", " << originY << ")" << std::endl;
            exit(1);
        }*/

        // We delete the children
        delete northeast;
        delete northwest;
        delete southeast;
        delete southwest;
        //std::cerr << "Deleted the quadtree at (" << originX << ", " << originY << ") done" << std::endl;
    }/* else {
        std::cerr << "Deleting empty quadtree" << std::endl;
    }*/

    particle = nullptr;

    isDivided = false;
    hasBody = false;
    weightBranch = 0;
    width = 0;
    originX = 0;
    originY = 0;

    // We set the children to nullptr
    northeast = nullptr;
    northwest = nullptr;
    southeast = nullptr;
    southwest = nullptr;
    // particles = nullptr; // We do not delete the particles listes
}

bool QuadTree::buildTree() {
    // We build the tree
    if (particles == nullptr) {
        return false;
    }

    // We reset the tree if it was already built
    if (isDivided) {
        //if (debugMode()) std::cout << "Recycling the quadtree" << std::endl;
        // We recycle the quadtree to avoid memory allocation and deallocation but only the children
        // Because we are in the root node
        if (northeast != nullptr) northeast->recycle();
        if (northwest != nullptr) northwest->recycle();
        if (southeast != nullptr) southeast->recycle();
        if (southwest != nullptr) southwest->recycle();
        // We reset the root node
        reset();
    }

    //if (debugMode()) std::cout << "Compute the quadtree base width" << std::endl;

    // Sort particles by Morton code or spatial grid
    /*std::sort(particles->begin(), particles->end(), 
    [](Particle* a, Particle* b) {
        return (a->getX() < b->getX()) || (a->getX() == b->getX() && a->getY() < b->getY());
    });*/

    // We need to calculate the width of the newWindow of simulation
    // For that we need to calculate the maximum distance from the origin
    width = 0.0;
    // #pragma omp parallel for reduction(max:width) TODO future step
    for (Particle* particle : *particles) {
        double distance = std::max(std::abs(particle->getX()), std::abs(particle->getY()));
        if (distance > width) {
            width = distance;
        }
    }

    // We do x2 for minus and plus
    width *= 2;
    // We do a little bit more to avoid particles to be on the edge
    width *= 1.1;

    //if (debugMode()) std::cout << "Building the quadtree" << std::endl;
    // We insert the particles into the quadtree
    // PUT semaphore for the insertion on the different branches (QuadTree)
    for (Particle* particle : *particles) {
        insertSimple(particle);
    }

    return true;
}

bool QuadTree::isItDivided() {
    return isDivided;
}

Particle* QuadTree::getParticle() const {
    return particle;
}

bool QuadTree::hasParticle() {
    return hasBody;
}

const int& QuadTree::getWeightBranch() const {
    return weightBranch;
}

const double& QuadTree::getWidth() const {
    return width;
}

const double& QuadTree::getOriginX() const {
    return originX;
}

const double& QuadTree::getOriginY() const {
    return originY;
}

std::vector<Particle*>* QuadTree::getParticlesList() const {
    return particles;
}

QuadTree* QuadTree::getNortheast() const {
    return northeast;
}

QuadTree* QuadTree::getNorthwest() const {
    return northwest;
}

QuadTree* QuadTree::getSoutheast() const {
    return southeast;
}

QuadTree* QuadTree::getSouthwest() const {
    return southwest;
}

void QuadTree::setupMPIValues(int* rank, int* size) {
    rankMPI = rank;
    sizeMPI = size;
}