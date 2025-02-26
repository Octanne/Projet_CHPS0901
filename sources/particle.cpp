#include "particle.h"

#include <cmath>

Particle::Particle(double x, double y, double mass) : x(x), y(y), mass(mass) {}

Particle::Particle(const Particle& particle) : x(particle.x), y(particle.y), mass(particle.mass) {}

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

std::vector<Particle*> Particle::generateParticles(int nb_particles, double width, double height, double max_mass, double min_mass) {
    std::vector<Particle*> particles;
    for (int i = 0; i < nb_particles; i++) {
        double x = (double)rand() / RAND_MAX * width;
        double y = (double)rand() / RAND_MAX * height;
        double mass = (double)rand() / RAND_MAX * (max_mass - min_mass) + min_mass;
        particles.push_back(new Particle(x, y, mass));
    }
    return particles;
}