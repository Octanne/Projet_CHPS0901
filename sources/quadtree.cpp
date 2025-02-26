#include "quadtree.h"
#include "particle.h"
#include <iostream>
#include <cmath>

QuadTree::QuadTree(double x, double y, int width, int originX, int originY)
    : particle(x,y), isDivided(false), hasBody(false), width(width),
      originX(originX), originY(originY), northeast(nullptr), 
      northwest(nullptr), southeast(nullptr), southwest(nullptr) {}

QuadTree::QuadTree(int width, int originX, int originY)
    : QuadTree(0, 0, width, originX, originY) {}

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
        std::cout << spacer(depth) << "Leaf at (" << particle.getX() << ", " << particle.getY() << ") with mass " << particle.getMass() << std::endl;
    }
}

// TODO adding the width in account for the insertion
void QuadTree::insert(Particle* particleInsert) {
    // If node x does not contain a body, put the new body b here.
    if (!hasBody && !isDivided) {
        particle.setX(particleInsert->getX());
        particle.setY(particleInsert->getY());
        particle.setMass(particleInsert->getMass());
        hasBody = true;
        return;
    }

    // If node x is an internal node, update the center-of-mass and total mass of x.
    if (isDivided) {
        updateCenterOfMass(particleInsert);
        // Recursively insert the body b in the appropriate quadrant.
        if (particleInsert->getX() < particle.getX()) { // We check for west quadrants
            if (particleInsert->getY() < particle.getY()) {
                southwest->insert(particleInsert);
            } else {
                northwest->insert(particleInsert);
            }
        } else { // We check for east quadrants
            if (particleInsert->getY() < particle.getY()) {
                southeast->insert(particleInsert);
            } else {
                northeast->insert(particleInsert);
            }
        }
        return;
    }

    // If node x is an external node, say containing a body named c
    if (hasBody && !isDivided) {
        // Subdivide the region further by creating four children.
        subdivide();

        // Recursively insert both b and c into the appropriate quadrant(s).
        Particle* existingParticle = &particle;
        hasBody = false; // The node is not a leaf anymore

        // We insert the existing particle into the appropriate quadrant
        if (existingParticle->getX() < particleInsert->getX()) { // We check for west quadrants
            if (existingParticle->getY() < particleInsert->getY()) {
                southwest->insert(existingParticle);
            } else {
                northwest->insert(existingParticle);
            }
        } else { // We check for east quadrants
            if (existingParticle->getY() < particleInsert->getY()) {
                southeast->insert(existingParticle);
            } else {
                northeast->insert(existingParticle);
            }
        } // TODO faut prendre en compte une width pour la taille d'un quadrant lors de l'insertion

        // Insert the new particle
        if (particleInsert->getX() < existingParticle->getX()) { // We check for west quadrants
            if (particleInsert->getY() < existingParticle->getY()) {
                southwest->insert(particleInsert);
            } else {
                northwest->insert(particleInsert);
            }
        } else { // We check for east quadrants
            if (particleInsert->getY() < existingParticle->getY()) {
                southeast->insert(particleInsert);
            } else {
                northeast->insert(particleInsert);
            }
        }

        // Finally, update the center-of-mass and total mass of x.
        updateCenterOfMass(particleInsert);
    }
}

void QuadTree::subdivide() {
    // We create the four quadrants
    int newWidth = width / 2;
    northeast = new QuadTree(newWidth, originX + newWidth, originY + newWidth);
    northwest = new QuadTree(newWidth, originX, originY + newWidth);
    southeast = new QuadTree(newWidth, originX + newWidth, originY);
    southwest = new QuadTree(newWidth, originX, originY);
    isDivided = true; // The node is set to divided
    hasBody = false; // The node is not a leaf anymore
}

void QuadTree::updateCenterOfMass(Particle* particleInsert) {
    double totalMass = particle.getMass() + particleInsert->getMass();
    particle.setX((particle.getX() * particle.getMass() + particleInsert->getX() * particleInsert->getMass()) / totalMass);
    particle.setY((particle.getY() * particle.getMass() + particleInsert->getY() * particleInsert->getMass()) / totalMass);
    particle.setMass(totalMass);
    //std::cout << "Center of mass updated" << std::endl;
}

void QuadTree::calculateForce(Particle* particleTest, double& fx, double& fy) const {
    if (particle.getMass() == 0 || (particle.getX() == particleTest->getX() && particle.getY() == particleTest->getY())) {
        return;
    }

    double dx = particle.getX() - particleTest->getX();
    double dy = particle.getY() - particleTest->getY();
    double dist = distance(particle.getX(), particle.getY(), particleTest->getX(), particleTest->getY());

    if (!isDivided && dist > 0) {
        double force = (particle.getMass() * particleTest->getMass()) / (dist * dist);
        fx += force * (dx / dist);
        fy += force * (dy / dist);
    } else if (dist < theta) {
        double force = (particle.getMass() * particleTest->getMass()) / (dist * dist);
        fx += force * (dx / dist);
        fy += force * (dy / dist);
    } else {
        if (northeast) northeast->calculateForce(particleTest, fx, fy);
        if (northwest) northwest->calculateForce(particleTest, fx, fy);
        if (southeast) southeast->calculateForce(particleTest, fx, fy);
        if (southwest) southwest->calculateForce(particleTest, fx, fy);
    }
}

double QuadTree::distance(double x1, double y1, double x2, double y2) const {
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool QuadTree::isItDivided() {
    return isDivided;
}

Particle* QuadTree::getParticle() {
    return &particle;
}

bool QuadTree::hasParticle() {
    return hasBody;
}

int QuadTree::getWidth() {
    return width;
}

int QuadTree::getOriginX() {
    return originX;
}

int QuadTree::getOriginY() {
    return originY;
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
    hasBody = false;
    originX = 0;
    originY = 0;
    width = 0;
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