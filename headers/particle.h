#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <string>

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

    //static constexpr double G = 6.6743e-11; // Gravitational constant
    static constexpr double G = 6.6743e-31;  // Gravitational constant
    
    /**
     * Generates a vector of Particle pointers with random positions and masses.
     *
     * @param nb_particles The number of particles to generate.
     * @param width The width of the area in which particles are generated.
     * @param height The height of the area in which particles are generated.
     * @param max_mass The maximum mass of the particles.
     * @param min_mass The minimum mass of the particles.
     * @return A vector of Particle pointers.
     */
    static std::vector<Particle*> generateParticles(int nb_particles, double width, double height, double max_mass, double min_mass);

    /**
     * Loads particles from a file.
     *
     * @param filename The name of the file to load particles from.
     * @return A vector of Particle pointers.
     * @example
     * The file should have the following format:
     * x1 y1 mass1
     * x2 y2 mass2
     * ...
     * Each line represents a particle with its x position, y position, and mass.
     */
    static std::vector<Particle*> loadParticles(std::string& filename);
    /**
     * @brief Saves particles to a file.
     * 
     * @param filename The name of the file to save particles to.
     * @param particles The vector of Particle pointers to save.
     */
    static void saveParticles(std::string& filename, std::vector<Particle*>& particles);
private:
    double x;
    double y;

    double mass;

    double vx;
    double vy;
};

#endif // PARTICLE_H