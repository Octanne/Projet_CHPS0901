#include "particle.h"

#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>

Particle::Particle(double x, double y, double mass) : x(x), y(y), mass(mass), vx(0), vy(0) {}

Particle::Particle(const Particle& particle) : x(particle.x), y(particle.y), mass(particle.mass), 
                        vx(particle.vx), vy(particle.vy) {}

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

void Particle::setVx(double vx) {
    this->vx = vx;
}

void Particle::setVy(double vy) {
    this->vy = vy;
}

double Particle::getVx() const {
    return vx;
}

double Particle::getVy() const {
    return vy;
}

std::vector<Particle*> Particle::generateParticles(int nb_particles, double width, double height, double max_mass, double min_mass) {
    std::vector<Particle*> particles;
    for (int i = 0; i < nb_particles; i++) {
        double x = (double)rand() / RAND_MAX * width;
        double y = (double)rand() / RAND_MAX * height;
        double mass = (double)rand() / RAND_MAX * (max_mass - min_mass) + min_mass;
        double vx = (double)rand() / RAND_MAX * 2 - 1;
        double vy = (double)rand() / RAND_MAX * 2 - 1;
        Particle* particle = new Particle(x, y, mass);
        particle->setVx(vx);
        particle->setVy(vy);
        particles.push_back(particle);
    }
    return particles;
}

std::vector<Particle*> Particle::loadParticles(std::string& filename) {
    std::vector<Particle*> particles;
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.rfind("//", 0) == 0 || line.empty()) continue;
            
            std::istringstream iss(line);
            iss.imbue(std::locale::classic());  // Ensure parsing of scientific notation
            
            double x, y, vx, vy, mass;
            if (!(iss >> x >> y >> vx >> vy >> mass)) {
                std::cerr << "Error while reading the file: " << line << std::endl;
                continue;
            }
            Particle* particle = new Particle(x, y, mass);
            particle->setVx(vx);
            particle->setVy(vy);
            particles.push_back(particle);
        }
        file.close();
    } else {
        std::cerr << "Error while opening the file" << std::endl;
    }
    return particles;
}

void Particle::saveParticles(std::string& filename, std::vector<Particle*>& particles) {
    // Create the folder if it does not exist
    std::string folder = filename.substr(0, filename.find_last_of("/\\"));
    if (!folder.empty()) {
        if (system(("mkdir -p " + folder).c_str()) == 0) {
            std::cout << "Folder created: " << folder << std::endl;
        }
    }
    std::ofstream file(filename);
    if (file.is_open()) {
        for (Particle* particle : particles) {
            file << particle->getX() << " " << particle->getY() << " " << particle->getVx() << " " << particle->getVy() << " " << particle->getMass() << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Error while opening the file" << std::endl;
    }
}