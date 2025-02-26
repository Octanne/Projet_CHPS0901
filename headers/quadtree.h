#ifndef QUADTREE_H
#define QUADTREE_H

#include "particle.h"

// Forward declaration of QuadTree class
class QuadTree;

// QuadTree class to represent the quadtree
class QuadTree {
private:
    Particle particle;

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
    QuadTree(double x, double y, double width, double originX, double originY);
    QuadTree(double width, double originX, double originY);
    ~QuadTree();

    void calculateForce(Particle* particle, double& fx, double& fy) const;
    double distance(double x1, double y1, double x2, double y2) const;
    bool isItDivided();
    bool hasParticle();

    double getOriginX();
    double getOriginY();
    double getWidth();
    Particle* getParticle();

    void insert(Particle* particleInsert);
    void subdivide();
    void updateCenterOfMass(Particle* particleInsert);
    void print(int depth = 0);
    void clear();

    QuadTree* getNortheast();
    QuadTree* getNorthwest();
    QuadTree* getSoutheast();
    QuadTree* getSouthwest();
};

#endif // QUADTREE_H