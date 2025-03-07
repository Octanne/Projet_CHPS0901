#include "quadtree.h"
#include "particle.h"

#include <iostream>
#include <cmath>

bool* QuadTree::debugModePtr = nullptr;
int* QuadTree::rankMPI = nullptr;
int* QuadTree::sizeMPI = nullptr;

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
    northwest = new QuadTree(newWidth, originX - center, originY + center, 0);
    northeast = new QuadTree(newWidth, originX + center, originY + center, 0);
    southwest = new QuadTree(newWidth, originX - center, originY - center, 0);
    southeast = new QuadTree(newWidth, originX + center, originY - center, 0);
    isDivided = true; // The node is set to divided
    hasBody = false; // The node is not a leaf anymore

    // Recursively insert the body b in the appropriate quadrant.
    Particle* particleToMove = particle;
    particle = new Particle();
    //std::cout << "Subdivided the quadtree origin at (" << originX << ", " << originY << ") with width " << width << std::endl;
    insertSimple(particleToMove);
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
    // If the current node is a an external node (and not b), calculate the force exerted by the current node on b, and add this amount to b's net force.
    if (!isDivided) {
        if (!hasBody) return; // If the node is empty, we return
        if (b != this->particle) {
            // We call the function to calculate the force between the two particles and add it to the net force of the particle.
            calcForce(b, particle, fx, fy);
        }
    } else {
        // Otherwise, calculate the ratio s / d, where s is the width of the region represented by the internal node, and d is the distance between the body and the node's center-of-mass.
        double s = width;
        double d = distance(b->getX(), b->getY(), particle->getX(), particle->getY());
        // If ratio > theta we perform recursively on each of the current node's children.
        if (s / d < theta) { 
            // Treat the internal node as a single body, and calculate the force it exerts on body b, and add this amount to b's net force.
            // We call the function to calculate the force between the two particles and add it to the net force of the particle.
            calcForce(b, particle, fx, fy);
        } 
        // Otherwise, treat it as a single body, and calculate the force it exerts on body b, and add this amount to b's net force.
        else {
            if (northeast != nullptr) northeast->calculateForce(b, fx, fy);
            if (northwest != nullptr) northwest->calculateForce(b, fx, fy);
            if (southeast != nullptr) southeast->calculateForce(b, fx, fy);
            if (southwest != nullptr) southwest->calculateForce(b, fx, fy);
        }
    }
}

// TODO MAYBE DO A FUSION OF PARTICLES WHEN THEY COLLIDE TO MAKE THEM SPREAD
// in multiple particles but we keep the mass total to avoid losing matter (energy conservation)
// We can also do a fusion of particles when they are too close to each other
// We can also do a fusion of particles when they are too close to the center of mass TO AVOID 
// THE COLLAPSE OF THE SYSTEM (To much divided sectors)

void QuadTree::computePosOfSubtree(QuadTree* tree, int *posOfSubtree, int& rankUsed, int branchNumber, int& nbParticleCovered) {
    // posOfSubtree is a array of sizeMPI
    // We compute the position of the start of the subtree to be handled by each rank
    // We have on weightBranch the number of particles in the branch
    // We have sizeMPI the number of ranks
    // We have rankMPI the rank of the current rank
    // We have tree the tree to compute the position of the subtree
    // We have posOfSubtree the array to store the position of the start of the subtree for each rank
    int branchParticle = tree->getWeightBranch();

    int particlesPerRank = weightBranch / *sizeMPI;

    // If the number of particle is less than the number of ranks we give the branch to the rank



    // If there is only one rank left then we give the whole branch to the rank
    if (rankUsed == *sizeMPI - 1) {
        posOfSubtree[rankUsed] = branchNumber;
        rankUsed++;
        nbParticleCovered += branchParticle;
        return;
    }
    // If the branch has more than 1/sizeMPI but less than 2/sizeMPI of the total particles 
    // we enter in the branch and give the part to the rank.
    if (branchParticle >= particlesPerRank && branchParticle < 2 * particlesPerRank) {
        // We give the branch to the rank
        posOfSubtree[rankUsed] = branchNumber;
        rankUsed++;
        nbParticleCovered += branchParticle;
        return;
    // We only enter in the branch if the branch has more than 2/sizeMPI of the total particles because 
    // the branch will just have less particles if we continue to go deeper in the tree
    } else if (branchParticle >= 2 * particlesPerRank) {
        // We enter in one of the quadrants
        if (tree->getNortheast() != nullptr) {
            computePosOfSubtree(tree->getNortheast(), posOfSubtree, rankUsed, branchNumber * 4 + 1, nbParticleCovered);
        }
        if (tree->getNorthwest() != nullptr && nbParticleCovered < weightBranch) {
            computePosOfSubtree(tree->getNorthwest(), posOfSubtree, rankUsed, branchNumber * 4 + 2, nbParticleCovered);
        }
        if (tree->getSoutheast() != nullptr && nbParticleCovered < weightBranch) {
            computePosOfSubtree(tree->getSoutheast(), posOfSubtree, rankUsed, branchNumber * 4 + 3, nbParticleCovered);
        }
        if (tree->getSouthwest() != nullptr && nbParticleCovered < weightBranch) {
            computePosOfSubtree(tree->getSouthwest(), posOfSubtree, rankUsed, branchNumber * 4 + 4, nbParticleCovered);
        }
    } else {
        // We do not enter in the branch
        return;
    }
}

void QuadTree::updateParticles(double step) {
    int posOfSubtree = 0;
    
    // We compute the position of the start of the subtree to be handled by each rank
    int* posOfSubtreeArray = new int[*sizeMPI];
    int rankUsed = 0;
    int nbParticlesCovered = 0;
    computePosOfSubtree(this, posOfSubtreeArray, rankUsed, posOfSubtree, nbParticlesCovered);
    if (rankMPI == 0) {
        for (int i = 0; i < *sizeMPI; i++) {
            std::cout << "Rank " << i << " has to handle the branch " << posOfSubtreeArray[i] << std::endl;
        }
        std::cout << "Number of particles covered " << nbParticlesCovered << "out of " << weightBranch << std::endl;
    }

    // HEAD NODE
    if (rankMPI != nullptr && *rankMPI == 0) {
        for (Particle* particle : *particles) {
            double fx = 0.0, fy = 0.0;
            

        }

    }

    // WORKER NODE



    // Update velocity of the particles
    // CAN BE OPENMPized
    for (Particle* particle : *particles) {
        double fx = 0.0, fy = 0.0;
        calculateForce(particle, fx, fy);
        // We print the force exerted on the particle
        if (debugMode()) std::cout << "Particle at (" << particle->getX() << ", " << particle->getY() 
            << ") has force (" << fx << ", " << fy << ")" << std::endl;

        particle->setVx(particle->getVx() + fx * step);
        particle->setVy(particle->getVy() + fy * step);

        // We print the velocity of the particle
        if (debugMode()) std::cout << "Particle at (" << particle->getX() << ", " << particle->getY()
             << ") has velocity (" << particle->getVx() << ", " << particle->getVy() << ")" << std::endl;
    }
    // Update position of the particles
    // Can be OPENMPized
    for (Particle* particle : *particles) {
        particle->setX(particle->getX() + particle->getVx() * step);
        particle->setY(particle->getY() + particle->getVy() * step);
        if (debugMode()) std::cout << "Particle now at (" << particle->getX() << ", " << particle->getY() 
            << ")" << std::endl;
    }
}

QuadTree::~QuadTree() {
    clear();
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
        if (debugMode()) std::cout << "Clearing the quadtree" << std::endl;
        clear();
    }

    if (debugMode()) std::cout << "Compute the quadtree base width" << std::endl;

    // We need to calculate the width of the newWindow of simulation
    // For that we need to calculate the maximum distance from the origin
    width = 0.0;
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

    if (debugMode()) std::cout << "Building the quadtree" << std::endl;
    // We insert the particles into the quadtree
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