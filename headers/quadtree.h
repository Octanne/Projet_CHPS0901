#ifndef QUADTREE_H
#define QUADTREE_H

#include "particle.h"

// Forward declaration of QuadTree class
class QuadTree;

// QuadTree class to represent the quadtree
class QuadTree {
private:
    double particleX;
    double particleY;
    double mass;

    bool isDivided;
    bool hasBody;

    QuadTree* northeast;
    QuadTree* northwest;
    QuadTree* southeast;
    QuadTree* southwest;

    static constexpr double theta = 0.5;  // Barnes-Hut threshold parameter
public:
    QuadTree(double x = 0.f, double y = 0.f);
    ~QuadTree();

    void calculateForce(Particle* particle, double& fx, double& fy) const;
    double distance(double x1, double y1, double x2, double y2) const;
    bool isItDivided();
    bool hasParticle();

    double getX();
    double getY();
    double getMass();

    void insert(Particle* particle);
    void subdivide();
    void updateCenterOfMass(Particle* particle);
    void clear();

    QuadTree* getNortheast();
    QuadTree* getNorthwest();
    QuadTree* getSoutheast();
    QuadTree* getSouthwest();
};

#endif // QUADTREE_H