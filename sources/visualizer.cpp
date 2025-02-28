#include "visualizer.h"

#include <iostream>
#include <unistd.h>
#include <thread>
#include <csignal>

// install sudo apt install libglew-dev libglfw3-dev libglm-dev
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Global QuadTree pointer for display callback
std::thread* windowThread = nullptr;
bool shouldClose = false;
bool renderBoundaries = true;
bool debugMode = false;
double* windowSize;

// Recursively draw the QuadTree
int drawQuadTree(QuadTree* qt, double simulationSize, int root = 0) {
    if (qt == nullptr) {
        std::cout << "Quad is null" << std::endl;
        return 0;
    }
    int nbPart = 0;
    double scaleFactor = (*windowSize)/simulationSize;
    double width = qt->getWidth()*scaleFactor*0.5;

    // Print scale factor and width
    //if (root) std::cout << "Scale factor: " << scaleFactor << " Width: " << width << std::endl;

    // Draw boundaries
    if (renderBoundaries) {
        double posX = qt->getOriginX()*scaleFactor;
        double posY = qt->getOriginY()*scaleFactor;
        //std::cout << "Box origin at (" << posX << ", " << posY << ")" << std::endl;
        //printf("Drawing boundaries at (%f, %f, %f, %f)\n", posX-width, posY+width, posX+width, posY-width);
        glBegin(GL_LINE_LOOP);
        glColor3f(0.0, 0.0, 1.0);
        glVertex2f(posX-width, posY+width);
        glVertex2f(posX+width, posY+width);
        glVertex2f(posX+width, posY-width);
        glVertex2f(posX-width, posY-width);
        glEnd();
    }

    if (qt->isItDivided()) {
        // Draw other Quads
        nbPart += drawQuadTree(qt->getSouthwest(), simulationSize);
        nbPart += drawQuadTree(qt->getSoutheast(), simulationSize);
        nbPart += drawQuadTree(qt->getNorthwest(), simulationSize);
        nbPart += drawQuadTree(qt->getNortheast(), simulationSize);
    } else if (qt->hasParticle()) {
        // Draw particle
        //printf("Drawing particle at (%f, %f, %f)\n", qt->getX(), qt->getY(), qt->getMass());
        glPointSize(3.0);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);
        double posX = qt->getParticle()->getX()*scaleFactor;
        double posY = qt->getParticle()->getY()*scaleFactor;
        glVertex2f(posX, posY);
        glEnd();
        return nbPart+1;
    }

    if (root == 1) {}/*std::cout << "We printed " << nbPart << " particles" << std::endl;*/

    return nbPart;
}

// Display callback function
void displayCallback(QuadTree* qt, double simulationSize) {
    if (qt != nullptr) {
        drawQuadTree(qt, simulationSize, 1);
        // Usleep for 60 fps
        usleep(1000000/60);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {  
    *windowSize = width;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, (*windowSize), -1, (*windowSize)+1, -1, 1);
    // I want to lock aspect ratio
    glfwSetWindowAspectRatio(window, 1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width, height);
  
    std::cout << "Resizing of window id " << glfwGetWindowAttrib(window, GLFW_FOCUSED) << " to " << width << "x" << height << std::endl;
}

// Wait for the window to be closed
void waitClosedWindow() {
    windowThread->join();
}

GLFWwindow* window_GEN;
void signalSIGINTHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received. Closing window" << std::endl;
    glfwSetWindowShouldClose(window_GEN, GLFW_TRUE);
    shouldClose = true;
}

// Initialize and create window
int createWindow(QuadTree* qt, double windowDefaultSize, double simulationSize) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(windowDefaultSize, windowDefaultSize, "QuadTree Visualizer", nullptr, nullptr);
    window_GEN = window;
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(NULL);

    windowSize = new double(windowDefaultSize);

    windowThread = new std::thread([window, qt, simulationSize]() {
        // Create window
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            return;
        }

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, (*windowSize), -1, (*windowSize)+1, -1, 1);
        // I want to lock aspect ratio
        glfwSetWindowAspectRatio(window, 1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Capture CTRL+C to stop gracefully
        signal(SIGINT, signalSIGINTHandler);

        // Set the key callback
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (debugMode) std::cout << "(" << &window << ") Key pressed: " << key << "with scancode " << scancode << " with action " << action << " with mods " << mods << std::endl;
            // Close window when Q is pressed
            if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
                signalSIGINTHandler(2);
            }
            // Switch render boundaries
            else if (key == GLFW_KEY_F2 && action == GLFW_RELEASE) {
                renderBoundaries = !renderBoundaries;
                std::cout << "Render boundaries: " << (renderBoundaries ? "ON" : "OFF") << std::endl;
            }
        });

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);
            displayCallback(qt, simulationSize);
            glfwSwapBuffers(window);
            glfwPollEvents();
            // We sleep for 100ms
            usleep(100000);
        }

        // Remove context from current thread
        glfwMakeContextCurrent(NULL);
        glfwDestroyWindow(window);
        glfwTerminate();

        delete windowSize;

        std::cout << "Window closed" << std::endl;
    });

    return 0;
}
