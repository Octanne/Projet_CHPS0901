#include "quadtree.h"
#include "particle.h"
#include <iostream>
#include <cmath>

QuadTree::QuadTree(double x, double y, int width)
    : particleX(x), particleY(y), mass(0), 
      isDivided(false), hasBody(false), width(width),
      northeast(nullptr), northwest(nullptr), 
      southeast(nullptr), southwest(nullptr) {}

QuadTree::QuadTree(int width)
    : QuadTree(0, 0, width) {}

QuadTree::~QuadTree() {
    clear();
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
        std::cout << spacer(depth) << "Leaf at (" << particleX << ", " << particleY << ") with mass " << mass << std::endl;
    }
}

void QuadTree::insert(Particle* particle) {
    // If node x does not contain a body, put the new body b here.
    if (!hasBody && !isDivided) {
        particleX = particle->getX();
        particleY = particle->getY();
        mass = particle->getMass();
        hasBody = true;
        return;
    }

    // If node x is an internal node, update the center-of-mass and total mass of x.
    if (isDivided) {
        updateCenterOfMass(particle);
        // Recursively insert the body b in the appropriate quadrant.
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
        return;
    }

    // If node x is an external node, say containing a body named c
    if (hasBody && !isDivided) {
        // Subdivide the region further by creating four children.
        subdivide();

        // Recursively insert both b and c into the appropriate quadrant(s).
        Particle* existingParticle = new Particle(particleX, particleY, mass);
        hasBody = false; // The node is not a leaf anymore

        // We insert the existing particle into the appropriate quadrant
        if (existingParticle->getX() < particle->getX()) { // We check for west quadrants
            if (existingParticle->getY() < particle->getY()) {
                southwest->insert(existingParticle);
            } else {
                northwest->insert(existingParticle);
            }
        } else { // We check for east quadrants
            if (existingParticle->getY() < particle->getY()) {
                southeast->insert(existingParticle);
            } else {
                northeast->insert(existingParticle);
            }
        } // TODO faut prendre en compte une width pour la taille d'un quadrant lors de l'insertion

        // Insert the new particle
        if (particle->getX() < existingParticle->getX()) { // We check for west quadrants
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

        // Finally, update the center-of-mass and total mass of x.
        updateCenterOfMass(particle);
    }
}

void QuadTree::subdivide() {
    // We create the four quadrants
    int newWidth = width / 2;
    northeast = new QuadTree(newWidth);
    northwest = new QuadTree(newWidth);
    southeast = new QuadTree(newWidth);
    southwest = new QuadTree(newWidth);
    isDivided = true; // The node is set to divided
    hasBody = false; // The node is not a leaf anymore
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

double QuadTree::getWidth() {
    return width;
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