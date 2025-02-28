#include "quadtree.h"
#include "particle.h"
#include <iostream>
#include <cmath>

QuadTree::QuadTree(double width, double originX, double originY, std::vector<Particle*>* particles)
    : particle(nullptr), isDivided(false), hasBody(false), width(width),
      originX(originX), originY(originY), northeast(nullptr), northwest(nullptr), 
      southeast(nullptr), southwest(nullptr), particles(particles) {}

QuadTree::QuadTree(double width, double originX, double originY)
    : particle(nullptr), isDivided(false), hasBody(false), width(width),
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
    insertSimple(particleToMove);
}

void QuadTree::updateCenterOfMass(Particle* particleInsert) {
    double totalMass = particle->getMass() + particleInsert->getMass();
    particle->setX((particle->getX() * particle->getMass() + particleInsert->getX() * particleInsert->getMass()) / totalMass);
    particle->setY((particle->getY() * particle->getMass() + particleInsert->getY() * particleInsert->getMass()) / totalMass);
    particle->setMass(totalMass);
    //std::cout << "Center of mass updated" << std::endl;
}

void QuadTree::calculateForce(Particle* b, double& fx, double& fy) const {
    // If the current node is a an external node (and not b), calculate the force exerted by the current node on b, and add this amount to b's net force.
    if (!isDivided) {
        if (!hasBody) return; // If the node is empty, we return
        if (b != this->particle) {
            // We calculate the force exerted by the current node on b
            double dx = b->getX() - particle->getX();
            double dy = b->getY() - particle->getY();
            double distance = std::sqrt(dx * dx + dy * dy);
            double force = (Particle::G * particle->getMass() * b->getMass()) / (distance * distance);
            fx += force * dx / distance;
            fy += force * dy / distance;
        }
    } else {
        // Otherwise, calculate the ratio s / d, where s is the width of the region represented by the internal node, and d is the distance between the body and the node's center-of-mass.
        double s = width;
        double d = distance(particle->getX(), particle->getY(), b->getX(), b->getY());
        if (s / d < theta) {
            // Treat the internal node as a single body, and calculate the force it exerts on body b, and add this amount to b's net force.
            double dx = b->getX() - particle->getX();
            double dy = b->getY() - particle->getY();
            double distance = std::sqrt(dx * dx + dy * dy);
            double force = (Particle::G * particle->getMass() * b->getMass()) / (distance * distance);
            fx += force * dx / distance;
            fy += force * dy / distance;
        } else {
            // Recursively call the same function on each of the current node's children.
            if (northeast != nullptr) northeast->calculateForce(b, fx, fy);
            if (northwest != nullptr) northwest->calculateForce(b, fx, fy);
            if (southeast != nullptr) southeast->calculateForce(b, fx, fy);
            if (southwest != nullptr) southwest->calculateForce(b, fx, fy);
        }
    }

    // We print the force exerted on the particle
    std::cout << "Particle at (" << particle->getX() << ", " << particle->getY() << ") has force (" << fx << ", " << fy << ")" << std::endl;
}

void QuadTree::updateParticles(double step) {
    for (Particle* particle : *particles) {
        double fx = 0.0, fy = 0.0;
        calculateForce(particle, fx, fy);
        std::cout << "Force calculated" << std::endl;

        double ax = fx / particle->getMass();
        double ay = fy / particle->getMass();

        particle->setVx(particle->getVx() + ax * step);
        particle->setVy(particle->getVy() + ay * step);
        
        // We print the velocity of the particle
        std::cout << "Particle at (" << particle->getX() << ", " << particle->getY() << ") has velocity (" 
                        << particle->getVx() << ", " << particle->getVy() << ")" << std::endl;

        particle->setX(particle->getX() + particle->getVx() * step);
        particle->setY(particle->getY() + particle->getVy() * step);
    }
}

double QuadTree::distance(double x1, double y1, double x2, double y2) const {
    // Calculate the distance between two points 
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool QuadTree::buildTree() {
    // We build the tree
    if (particles == nullptr) {
        return false;
    }

    // We need to calculate the width of the newWindow of simulation
    // For that we need to calculate the maximum distance from the origin
    width = 0.0;
    for (Particle* particle : *particles) {
        double absX = std::abs(particle->getX());
        double absY = std::abs(particle->getY());
        if (absX > width) {
            width = absX;
        }
        if (absY > width) {
            width = absY;
        }
    }

    // We reset the tree if it was already built
    if (isDivided) {
        std::cout << "Clearing the quadtree" << std::endl;
        clear();
    }

    std::cout << "Building the quadtree" << std::endl;
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

QuadTree::~QuadTree() {
    clear();
}

void QuadTree::clear() {
    if (isDivided) {
        // We delete the particle if divided because it as been created by the quadtree
        delete particle;

        // We delete the children
        delete northeast;
        delete northwest;
        delete southeast;
        delete southwest;
    }

    particle = nullptr;
    isDivided = false;
    hasBody = false;
    // width = width
    // originX = originX
    // originY = originY
    northeast = nullptr;
    northwest = nullptr;
    southeast = nullptr;
    southwest = nullptr;
    // particles = nullptr; // We do not delete the particles listes
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