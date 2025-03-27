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

    void recycle(); // Reset and return to pool
    void reset(); // Reset the node
    static std::vector<QuadTree*> nodePool;
    static QuadTree* getNode(double width, double ox, double oy, int weightBranch);
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

    void updateParticles(double step, double *localAccX, double *localAccY, std::vector<Particle*> *particles); // Update the position of the particle step is in seconds
    void insert(Particle* particleInsert);
    void print(int depth = 0);
    void clear();

    std::vector<std::vector<QuadTree*>> computeBalancedRanks(int nRank) const;

    QuadTree* getNortheast() const;
    QuadTree* getNorthwest() const;
    QuadTree* getSoutheast() const;
    QuadTree* getSouthwest() const;

    static int* rankMPI;
    static int* sizeMPI;
    static bool* debugModePtr;
    static void setDebugModePtr(bool* debugMode);
    static bool debugMode();
    static void setupMPIValues(int* rank, int* size);    

};

#endif // QUADTREE_H