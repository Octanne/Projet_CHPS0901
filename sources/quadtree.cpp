#include "quadtree.h"
#include "particle.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>

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

void QuadTree::computePosOfSubtreeDynamic(QuadTree* tree, QuadTree** posOfSubtree, std::vector<int>& currentLoadPerRank, 
    int& rankUsed, int branchNumber, int& nbParticleCovered, int depth) {
    // Si le sous-arbre actuel n'a pas de particules, retour
    if (tree == nullptr || !tree->hasParticle()) {
        return;
    }

    // Calcul du poids de l'arbre (nombre de particules dans ce sous-arbre)
    int subtreeWeight = tree->getWeightBranch();

    // Mise à jour du nombre total de particules couvertes
    nbParticleCovered += subtreeWeight;

    // Recherche du rank avec la charge la plus faible
    int minRank = std::distance(currentLoadPerRank.begin(), std::min_element(currentLoadPerRank.begin(), currentLoadPerRank.end()));

    // Si la charge de travail pour ce rank est trop importante, rééquilibrage nécessaire
    if (currentLoadPerRank[minRank] + subtreeWeight > nbParticleCovered / (rankUsed + 1)) {
        // Si un processus a trop de charge, on peut décider d'utiliser un autre processus
        minRank = (minRank + 1) % rankUsed;  // Répartition dynamique (peut ajuster en fonction d'autres critères)
    }

    // Attribuer le sous-arbre au processus (rank) identifié
    posOfSubtree[minRank] = tree;

    // Mettre à jour la charge de travail pour ce processus
    currentLoadPerRank[minRank] += subtreeWeight;

    // Incrémenter le nombre de ranks utilisés si nécessaire
    if (currentLoadPerRank[minRank] > nbParticleCovered / (rankUsed + 1) && rankUsed < branchNumber) {
        rankUsed++;
    }

    // Si l'arbre est divisé en sous-arbres (quadrants), appliquer récursivement aux sous-arbres
    if (tree->getNortheast() != nullptr) {
        computePosOfSubtreeDynamic(tree->getNortheast(), posOfSubtree, currentLoadPerRank, rankUsed, branchNumber, nbParticleCovered, depth + 1);
    }
    if (tree->getNorthwest() != nullptr) {
        computePosOfSubtreeDynamic(tree->getNorthwest(), posOfSubtree, currentLoadPerRank, rankUsed, branchNumber, nbParticleCovered, depth + 1);
    }
    if (tree->getSoutheast() != nullptr) {
        computePosOfSubtreeDynamic(tree->getSoutheast(), posOfSubtree, currentLoadPerRank, rankUsed, branchNumber, nbParticleCovered, depth + 1);
    }
    if (tree->getSouthwest() != nullptr) {
        computePosOfSubtreeDynamic(tree->getSouthwest(), posOfSubtree, currentLoadPerRank, rankUsed, branchNumber, nbParticleCovered, depth + 1);
    }
}


int QuadTree::computePosOfSubtreeHierarchical(QuadTree* tree, QuadTree** posOfSubtree, int& alreadyUsed, int& lastUsedRank, int& nbParticleCovered, int depth) {
    // On commence par regarder combien de particules sont couvertes par le sous-arbre
    int subtreeWeight = tree->getWeightBranch();
    int meanWeight = getWeightBranch() / *sizeMPI;
    std::cout << "Subtree weight " << subtreeWeight << " from depth " << depth << std::endl;
    
    // On ajoute la branche si le poids et <= au meanWeight
    if (subtreeWeight <= meanWeight && alreadyUsed < *sizeMPI) {
        std::cerr << "Subtree weight " << subtreeWeight << " is less than mean weight " << meanWeight << std::endl;
        posOfSubtree[lastUsedRank] = tree;
        lastUsedRank++;
        return 0;
    }

    // Si le nombre de processus est supérieur à 1, on doit répartir les sous-arbres
    // On commence par recuperer le nombre de sous arbres à traiter (non vide)
    int nbBranches = 0;
    if (isDivided){
        if (tree->northeast->getWeightBranch()) nbBranches++;
        if (tree->northwest->getWeightBranch()) nbBranches++;
        if (tree->southeast->getWeightBranch()) nbBranches++;
        if (tree->southwest->getWeightBranch()) nbBranches++;
    }

    // On mets à jour le nombre de processus utilisés si on a suffisamment de rank pour traiter les sous-arbres
    if (nbBranches+alreadyUsed <= *sizeMPI) {
        alreadyUsed += nbBranches;
    } else {
        return 1;
    }

    // Sinon on doit répartir les sous-arbres
    if (isDivided) {
        // On parcourt les sous-arbres du plus gros au plus petit
        std::vector<QuadTree*> branches;
        if (tree->weightBranch) branches.push_back(tree->northeast);
        if (tree->weightBranch) branches.push_back(tree->northwest);
        if (tree->weightBranch) branches.push_back(tree->southeast);
        if (tree->weightBranch) branches.push_back(tree->southwest);
        std::sort(branches.begin(), branches.end(), [](QuadTree* a, QuadTree* b) {
            return a->weightBranch > b->weightBranch;
        });

        // On parcourt les branches et on les attribue aux processus
        for (QuadTree* branch : branches) {
            int hasNotAddChild = computePosOfSubtreeHierarchical(branch, posOfSubtree, alreadyUsed, lastUsedRank, nbParticleCovered, depth + 1);
            if (hasNotAddChild) {
                posOfSubtree[lastUsedRank] = branch;
                lastUsedRank++;
            }
        }
    }

    return 0;
}

void QuadTree::updateParticles(double step) { 
    // We compute the position of the start of the subtree to be handled by each rank
    QuadTree* poOfSubtree[*sizeMPI];
    // we set to -1
    for (int i = 0; i < *sizeMPI; i++) poOfSubtree[i] = nullptr;
    int rankUsed = 0;
    int nbParticleCovered = 0;
    int lastUsedRank = 0;
    std::vector<int> currentLoadPerRank(*sizeMPI, 0);  // Charge de chaque processus
    computePosOfSubtreeHierarchical(this, poOfSubtree, rankUsed, lastUsedRank, nbParticleCovered, 0);
    if (*rankMPI == 0) {
        for (int i = 0; i < *sizeMPI; i++) {
            std::cout << "Rank " << i << " has to handle the branch " << poOfSubtree[i] << std::endl;
            std::cout << "Number of particles covered " << currentLoadPerRank[i] << std::endl;
            QuadTree* branch = poOfSubtree[i];
            std::cout << "Branch number " << poOfSubtree[i] << " width address " << branch << std::endl;
            std::cout << "Branch number " << poOfSubtree[i] << " has " << branch->getWeightBranch() << " particles" << std::endl;
            std::cout << "Branch number " << poOfSubtree[i] << " with pointer " << branch << std::endl;
        }
        std::cout << "Number of particles covered " << nbParticleCovered << " out of " << getWeightBranch() << std::endl;
    }

    // HEAD NODE
    /*if (rankMPI != nullptr && *rankMPI == 0) {
        for (Particle* particle : *particles) {
            double fx = 0.0, fy = 0.0;
            

        }

    }*/

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