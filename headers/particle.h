#ifndef PARTICLE_H
#define PARTICLE_H

class Particle {
public:
    Particle(double x = 0.0, double y = 0.0, double mass = 0.0);
    
    double getX() const;
    double getY() const;
    double getMass() const;

    void setX(double x);
    void setY(double y);
    void setMass(double mass);

private:
    double x;
    double y;

    double mass;
};

#endif // PARTICLE_H