#include "quadtree.h"
#include "particle.h"
#include <iostream>
#include <cmath>

QuadTree::QuadTree(double x, double y, double width, double originX, double originY)
    : particle(x,y), isDivided(false), hasBody(false), width(width),
      originX(originX), originY(originY), northeast(nullptr), 
      northwest(nullptr), southeast(nullptr), southwest(nullptr) {}

QuadTree::QuadTree(double width, double originX, double originY)
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
    if (!isDivided) {
        if (!hasBody) {
            particle.setX(particleInsert->getX());
            particle.setY(particleInsert->getY());
            particle.setMass(particleInsert->getMass());
            hasBody = true;
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
            southwest->insert(particleInsert);
            //std::cout << "SouthWest inserted particle at (" << particleInsert->getX() << ", " << particleInsert->getY() << ", " << particleInsert->getMass() << ")" << std::endl;
        } else {
            northwest->insert(particleInsert);
            //std::cout << "NorthWest inserted particle at (" << particleInsert->getX() << ", " << particleInsert->getY() << ", " << particleInsert->getMass() << ")" << std::endl;
        }
    } else { // We check for east quadrants
        if (particleInsert->getY() < getOriginY()) {
            southeast->insert(particleInsert);
            //std::cout << "SouthEast inserted particle at (" << particleInsert->getX() << ", " << particleInsert->getY() << ", " << particleInsert->getMass() << ")" << std::endl;
        } else {
            northeast->insert(particleInsert);
            //std::cout << "NorthEast inserted particle at (" << particleInsert->getX() << ", " << particleInsert->getY() << ", " << particleInsert->getMass() << ")" << std::endl;
        }
    }
}

void QuadTree::subdivide() {
    // We create the four quadrants
    double newWidth = width / 2;
    double center = newWidth / 2;
    northwest = new QuadTree(newWidth, originX - center, originY + center);
    northeast = new QuadTree(newWidth, originX + center, originY + center);
    southwest = new QuadTree(newWidth, originX - center, originY - center);
    southeast = new QuadTree(newWidth, originX + center, originY - center);
    isDivided = true; // The node is set to divided
    hasBody = false; // The node is not a leaf anymore

    // Recursively insert the body b in the appropriate quadrant.
    Particle existingParticle = Particle(particle);
    particle.setMass(0); particle.setX(0); particle.setY(0);
    //std::cout << "Subdivided the quadtree origin at (" << originX << ", " << originY << ") with width " << width << std::endl;
    insert(&existingParticle);
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

double QuadTree::getWidth() {
    return width;
}

double QuadTree::getOriginX() {
    return originX;
}

double QuadTree::getOriginY() {
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