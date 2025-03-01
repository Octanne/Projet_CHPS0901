#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "quadtree.h"
#include "thread"

// install sudo apt install libglew-dev libglfw3-dev libglm-dev
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Visualizer {
public:
    int createWindow();
    void waitClosedWindow();

    bool hasToClose() const;
    bool isInPause() const;

    bool setPause(bool pause);
    bool setDebug(bool debug);
    void closeWindow();
    static Visualizer* getInstance(QuadTree* qt, double windowDefaultSize);
    static Visualizer* getInstance();
    static bool* ptrDebugMode();
    static bool isInDebug();
private:
    Visualizer(QuadTree* qt, double windowDefaultSize);
    //~Visualizer();

    QuadTree* qt;
    double windowSize;
    bool shouldClose;
    bool shouldPause;
    bool renderBoundaries;
    std::thread* windowThread;
    GLFWwindow* window;

    int drawQuadTree(QuadTree* qt, double simulationSize, int root = 0);
    void displayDebugDataInWindow();
    void displayCallback();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void signalSIGINTHandler(int signum);
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static Visualizer* instance;
    static bool debugMode;
};

#endif // VISUALIZER_H