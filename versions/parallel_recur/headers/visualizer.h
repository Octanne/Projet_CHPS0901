#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "quadtree.h"
#include "thread"

#ifndef VISU
#define VISU 1
#endif

// install sudo apt install libglew-dev libglfw3-dev libglm-dev
#if VISU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#endif

class Visualizer {
public:
    #if VISU
    int createWindow();
    void waitClosedWindow();
    #endif

    bool hasToClose() const;
    bool isInPause() const;
    bool semLock();
    bool semUnlock();
    bool semCheck();

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
    #if VISU
    GLFWwindow* window;
    #endif

    int winX;
    int winY;
    double scaleFactor;
    bool fullRender;

    bool waitSem;
    bool accountedSem;

    #if VISU
    int drawQuadTreeArea(QuadTree* qt, int root = 0);
    int drawQuadTree(QuadTree* qt, int root = 0);
    void displayDebugDataInWindow();
    void displayCallback();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void signalSIGINTHandler(int signum);
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    #endif
    static Visualizer* instance;
    static bool debugMode;
    static bool windowDebugMode;
    static bool keyLeftPressed;
    static bool keyRightPressed;
    static bool keyUpPressed;
    static bool keyDownPressed;
    static bool keyMinusPressed;
    static bool keyPlusPressed;
};

#endif // VISUALIZER_H