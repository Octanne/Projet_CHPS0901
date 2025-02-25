#include "particle.h"

Particle::Particle(double x, double y, double mass) : x(x), y(y), mass(mass) {}

double Particle::getX() const {
    return x;
}

double Particle::getY() const {
    return y;
}

double Particle::getMass() const {
    return mass;
}

void Particle::setX(double x) {
    this->x = x;
}

void Particle::setY(double y) {
    this->y = y;
}

void Particle::setMass(double mass) {
    this->mass = mass;
}