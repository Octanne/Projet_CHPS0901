#include "quadtree.h"
#include "particle.h"
#include <iostream>
#include <cmath>

QuadTree::QuadTree(double x, double y)
    : particleX(x), particleY(y), mass(0), 
      isDivided(false), hasBody(false), northeast(nullptr), 
      northwest(nullptr), southeast(nullptr), 
      southwest(nullptr) {}

QuadTree::~QuadTree() {
    clear();
}

void QuadTree::insert(Particle* particle) {
    // The node don't contain any particle we insert the particle
    if (!isDivided) {
        if (!hasBody) {
            particleX = particle->getX();
            particleY = particle->getY();
            mass = particle->getMass();
            hasBody = true;
            //std::cout << "Particle inserted" << std::endl;
            return;
        }

        // The node is not divided and the node is not empty
        subdivide(particle);
        return;
    }

    // Update the mass of the node to take into account the new particle
    updateCenterOfMass(particle);

    /* The node is divided we go to the corresponding quadrant */
    // Insert the particle in the corresponding quadrant recursively
    if (particle->getX() < getX()) { // We check for west quadrants
        if (particle->getY() < getY()) {
            southwest->insert(particle);
        } else {
            northwest->insert(particle);
        }
    } else { // We check for east quadrants
        if (particle->getY() < getY()) {
            southeast->insert(particle);
        } else {
            northeast->insert(particle);
        }
    }
}

void QuadTree::subdivide(Particle* particleAdded) {
    Particle particleOldest(particleX, particleY, mass);

    // We create the four quadrants
    northeast = new QuadTree();
    northwest = new QuadTree();
    southeast = new QuadTree();
    southwest = new QuadTree();
    isDivided = true; // The node is set to divided
    hasBody = false; // The node is not a leaf anymore
    //std::cout << "Leaf expanding to four quadrants" << std::endl;

    // We insert the oldest particle and the new particle in the corresponding quadrant
    // We want them to be in different quadrants
    if (particleOldest.getX() < particleAdded->getX()) {
        if (particleOldest.getY() < particleAdded->getY()) {
            southwest->insert(&particleOldest);
            northeast->insert(particleAdded);
        } else {
            northwest->insert(&particleOldest);
            southeast->insert(particleAdded);
        }
    } else {
        if (particleOldest.getY() < particleAdded->getY()) {
            southeast->insert(&particleOldest);
            northwest->insert(particleAdded);
        } else {
            northeast->insert(&particleOldest);
            southwest->insert(particleAdded);
        }
    }
}

void QuadTree::updateCenterOfMass(Particle* particle) {
    double totalMass = mass + particle->getMass();
    particleX = (particleX * mass + particle->getX() * particle->getMass()) / totalMass;
    particleY = (particleY * mass + particle->getY() * particle->getMass()) / totalMass;
    mass = totalMass;
    //std::cout << "Center of mass updated" << std::endl;
}

void QuadTree::calculateForce(Particle* particle, double& fx, double& fy) const {
    if (mass == 0 || (particleX == particle->getX() && particleY == particle->getY())) {
        return;
    }

    double dx = particleX - particle->getX();
    double dy = particleY - particle->getY();
    double dist = distance(particleX, particleY, particle->getX(), particle->getY());

    if (!isDivided && dist > 0) {
        double force = (mass * particle->getMass()) / (dist * dist);
        fx += force * (dx / dist);
        fy += force * (dy / dist);
    } else if (dist < theta) {
        double force = (mass * particle->getMass()) / (dist * dist);
        fx += force * (dx / dist);
        fy += force * (dy / dist);
    } else {
        if (northeast) northeast->calculateForce(particle, fx, fy);
        if (northwest) northwest->calculateForce(particle, fx, fy);
        if (southeast) southeast->calculateForce(particle, fx, fy);
        if (southwest) southwest->calculateForce(particle, fx, fy);
    }
}

double QuadTree::distance(double x1, double y1, double x2, double y2) const {
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool QuadTree::isItDivided() {
    return isDivided;
}

double QuadTree::getX() {
    return particleX;
}

double QuadTree::getY() {
    return particleY;
}

double QuadTree::getMass() {
    return mass;
}

bool QuadTree::hasParticle() {
    return hasBody;
}

void QuadTree::clear() {
    delete northeast;
    delete northwest;
    delete southeast;
    delete southwest;

    northeast = nullptr;
    northwest = nullptr;
    southeast = nullptr;
    southwest = nullptr;

    isDivided = false;
    mass = 0;
}

QuadTree* QuadTree::getNortheast() {
    return northeast;
}

QuadTree* QuadTree::getNorthwest() {
    return northwest;
}

QuadTree* QuadTree::getSoutheast() {
    return southeast;
}

QuadTree* QuadTree::getSouthwest() {
    return southwest;
}