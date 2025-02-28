#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>

class Particle {
public:
    Particle(double x = 0.0, double y = 0.0, double mass = 0.0);
    Particle(const Particle& particle);
    
    double getX() const;
    double getY() const;
    double getMass() const;

    void setX(double x);
    void setY(double y);
    void setMass(double mass);

    void setVx(double vx);
    void setVy(double vy);
    
    double getVx() const;
    double getVy() const;

    static constexpr double G = 6.6743e-11;  // Gravitational constant
    static std::vector<Particle*> generateParticles(int nb_particles, double width, double height, double max_mass = 100, double min_mass = 0.5);
private:
    double x;
    double y;

    double mass;

    double vx;
    double vy;
};

#endif // PARTICLE_H