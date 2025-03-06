#ifndef QUADTREE_H
#define QUADTREE_H

#include "particle.h"
#include <vector>

// Forward declaration of QuadTree class
class QuadTree;

// QuadTree class to represent the quadtree
class QuadTree {
private: // TODO utiliser un pointeur pour la particule.
    Particle* particle;

    bool isDivided;
    bool hasBody;
    double width;

    int weightBranch;
    
    double originX;
    double originY;

    QuadTree* northeast;
    QuadTree* northwest;
    QuadTree* southeast;
    QuadTree* southwest;

    static constexpr double theta = 0.5;  // Barnes-Hut threshold parameter
    std::vector<Particle*>* particles; // Array of particles
    void insertSimple(Particle* particleInsert);
    void subdivide();
    void updateCenterOfMass(Particle* particleInsert);
public:
    QuadTree(double width, double originX, double originY, std::vector<Particle*>* particles);
    QuadTree(double width, double originX, double originY, int weightBranch);
    ~QuadTree();

    void calculateForce(Particle* b, double& fx, double& fy) const;
    double distance(double x1, double y1, double x2, double y2) const;
    bool buildTree();
    bool isItDivided();
    bool hasParticle();

    const int& getWeightBranch() const;
    const double& getOriginX() const;
    const double& getOriginY() const;
    const double& getWidth() const;
    Particle* getParticle() const;
    std::vector<Particle*>* getParticlesList() const;

    void updateParticles(double step = 0.01); // Update the position of the particle step is in seconds
    void insert(Particle* particleInsert);
    void print(int depth = 0);
    void clear();

    QuadTree* getNortheast() const;
    QuadTree* getNorthwest() const;
    QuadTree* getSoutheast() const;
    QuadTree* getSouthwest() const;

    static bool* debugModePtr;
    static void setDebugModePtr(bool* debugMode);
    static bool debugMode();
};

#endif // QUADTREE_H