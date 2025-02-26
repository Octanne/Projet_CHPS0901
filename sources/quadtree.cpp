#include "quadtree.h"
#include "particle.h"
#include <iostream>
#include <cmath>

QuadTree::QuadTree(double width, double originX, double originY)
    : particle(nullptr), isDivided(false), hasBody(false), width(width),
      originX(originX), originY(originY), northeast(nullptr), 
      northwest(nullptr), southeast(nullptr), southwest(nullptr) {}

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

// TODO adding the width in account for the insertion
void QuadTree::insert(Particle* particleInsert) {
    // If node x does not contain a body, put the new body b here.
    if (!isDivided) {
        if (!hasBody) {
            particle = particleInsert;
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
    Particle* particleToMove = particle;
    particle = new Particle();
    //std::cout << "Subdivided the quadtree origin at (" << originX << ", " << originY << ") with width " << width << std::endl;
    insert(particleToMove);
}

void QuadTree::updateCenterOfMass(Particle* particleInsert) {
    double totalMass = particle->getMass() + particleInsert->getMass();
    particle->setX((particle->getX() * particle->getMass() + particleInsert->getX() * particleInsert->getMass()) / totalMass);
    particle->setY((particle->getY() * particle->getMass() + particleInsert->getY() * particleInsert->getMass()) / totalMass);
    particle->setMass(totalMass);
    //std::cout << "Center of mass updated" << std::endl;
}

void QuadTree::calculateForce(Particle* b, double& fx, double& fy) const {
    if (!isDivided) {
        if (particle != nullptr && (particle->getX() != b->getX() || particle->getY() != b->getY())) {
            double dx = particle->getX() - b->getX();
            double dy = particle->getY() - b->getY();
            double dist = distance(particle->getX(), particle->getY(), b->getX(), b->getY());
            double force = (Particle::G * particle->getMass() * b->getMass()) / (dist * dist);
            // Change the formula to consider dist as tons of meters
            fx += force * (dx / dist);
            fy += force * (dy / dist);
        }
    } else {
        double s = width;
        double d = distance(originX, originY, b->getX(), b->getY());
        if (s / d < theta) {
            double dx = particle->getX() - b->getX();
            double dy = particle->getY() - b->getY();
            // dist is in ?
            double dist = distance(particle->getX(), particle->getY(), b->getX(), b->getY());
            double force = (Particle::G * particle->getMass() * b->getMass()) / (dist * dist);
            fx += force * (dx / dist);
            fy += force * (dy / dist);
        } else {
            if (northeast) northeast->calculateForce(b, fx, fy);
            if (northwest) northwest->calculateForce(b, fx, fy);
            if (southeast) southeast->calculateForce(b, fx, fy);
            if (southwest) southwest->calculateForce(b, fx, fy);
        }
    }
}

void QuadTree::updateVelocities(double step) {
    if (isDivided) {
        if (northeast != nullptr) northeast->updateVelocities(step);
        if (northwest != nullptr) northwest->updateVelocities(step);
        if (southeast != nullptr) southeast->updateVelocities(step);
        if (southwest != nullptr) southwest->updateVelocities(step);
    }
        
    if (particle != nullptr) {
        double fx = 0;
        double fy = 0;
        calculateForce(particle, fx, fy);
        std::cout << "Force on particle at (" << particle->getX() << ", " << particle->getY() << ") is (" << fx << ", " << fy << ")" << std::endl;
        double ax = fx / particle->getMass();
        double ay = fy / particle->getMass();
        particle->setVx(particle->getVx() + ax * step);
        particle->setVy(particle->getVy() + ay * step);

        // We update the position of the particle
        particle->setX(particle->getX() + particle->getVx() * step);
        particle->setY(particle->getY() + particle->getVy() * step);
    }
}

double QuadTree::distance(double x1, double y1, double x2, double y2) const {
    // Calculate the distance between two points 
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
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

const double& QuadTree::getWidth() const {
    return width;
}

const double& QuadTree::getOriginX() const {
    return originX;
}

const double& QuadTree::getOriginY() const {
    return originY;
}

QuadTree::~QuadTree() {
    clear();
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

    if (isDivided) {
        delete particle;
        particle = nullptr;
    }

    isDivided = false;
    hasBody = false;
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