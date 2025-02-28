#ifndef QUADTREE_H
#define QUADTREE_H

#include "particle.h"

// Forward declaration of QuadTree class
class QuadTree;

// QuadTree class to represent the quadtree
class QuadTree {
private: // TODO utiliser un pointeur pour la particule.
    Particle* particle;

    bool isDivided;
    bool hasBody;
    double width;
    double originX;
    double originY;

    QuadTree* northeast;
    QuadTree* northwest;
    QuadTree* southeast;
    QuadTree* southwest;

    static constexpr double theta = 0.5;  // Barnes-Hut threshold parameter
public:
    QuadTree(double width, double originX, double originY);
    ~QuadTree();

    void calculateForce(QuadTree* b, double& fx, double& fy) const;
    double distance(double x1, double y1, double x2, double y2) const;
    bool isItDivided();
    bool hasParticle();

    const double& getOriginX() const;
    const double& getOriginY() const;
    const double& getWidth() const;
    Particle* getParticle() const;

    void updateVelocities(double step = 0.01); // Update the position of the particle step is in seconds
    void insert(Particle* particleInsert);
    void subdivide();
    void updateCenterOfMass(Particle* particleInsert);
    void print(int depth = 0);
    void clear();

    QuadTree* getNortheast() const;
    QuadTree* getNorthwest() const;
    QuadTree* getSoutheast() const;
    QuadTree* getSouthwest() const;
};

#endif // QUADTREE_H