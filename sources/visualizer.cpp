#include "visualizer.h"

#include <unistd.h>
#include <iostream>
#include <csignal>
#include <fstream>
#include <vector>

// Set debug mode to false
bool Visualizer::debugMode = false;

Visualizer::Visualizer(QuadTree* qt, double windowDefaultSize) {
    this->qt = qt;
    this->windowSize = windowDefaultSize;
    this->shouldClose = false;
    this->shouldPause = false;
    this->renderBoundaries = false;
    this->windowThread = nullptr;
    this->window = nullptr;
}

// Recursively draw the QuadTree
int Visualizer::drawQuadTree(QuadTree* qt, double simulationSize, int root) {
    if (qt == nullptr) {
        std::cout << "Quad is null" << std::endl;
        return 0;
    }
    int nbPart = 0;
    double scaleFactor = windowSize/simulationSize;
    double width = qt->getWidth()*scaleFactor*0.5;

    // Print scale factor and width
    if (root && debugMode) std::cout << "Scale factor: " << scaleFactor << " Width: " << width << std::endl;

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
        glPointSize(5.0);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);
        double posX = qt->getParticle()->getX()*scaleFactor;
        double posY = qt->getParticle()->getY()*scaleFactor;
        glVertex2f(posX, posY);
        glEnd();
        return nbPart+1;
    }

    if (root == 1 && debugMode) std::cout << "We printed " << nbPart << " particles" << std::endl;

    return nbPart;
}

void Visualizer::displayDebugDataInWindow() {
    /*glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(10, windowSize - 20);
    std::string text = "shouldClose: " + std::to_string(shouldClose);
    drawText(text, 10, windowSize - 20, 1.0);
    glRasterPos2f(10, windowSize - 40);
    text = "shouldPause: " + std::to_string(shouldPause);
    drawText(text, 10, windowSize - 40, 1.0);
    glRasterPos2f(10, windowSize - 60);
    text = "renderBoundaries: " + std::to_string(renderBoundaries);
    drawText(text, 10, windowSize - 60, 1.0);
    glRasterPos2f(10, windowSize - 80);
    text = "debugMode: " + std::to_string(debugMode);
    drawText(text, 10, windowSize - 80, 1.0);
    glRasterPos2f(10, windowSize - 100);
    text = "windowSize: " + std::to_string(windowSize);
    drawText(text, 10, windowSize - 100, 1.0);*/
}

// Display callback function
void Visualizer::displayCallback() {
    if (qt != nullptr) {
        drawQuadTree(qt, qt->getWidth(), 1);
        // Usleep for 60 fps
        usleep(1000000/60);
        //if (debugMode) displayDebugDataInWindow();
    }
}

void Visualizer::waitClosedWindow() {
    windowThread->join();
}

void Visualizer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    instance->windowSize = width;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, instance->windowSize, -1, instance->windowSize+1, -1, 1);
    // I want to lock aspect ratio
    glfwSetWindowAspectRatio(window, 1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width, height);
  
    std::cout << "Resizing of window id " << glfwGetWindowAttrib(window, GLFW_FOCUSED) << " to " << width << "x" << height << std::endl;
}

GLFWwindow* window_GEN;
void Visualizer::signalSIGINTHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received. Closing window" << std::endl;
    glfwSetWindowShouldClose(window_GEN, GLFW_TRUE);
    instance->shouldClose = true;
}

void Visualizer::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (instance->debugMode) std::cout << "(" << &window << ") Key pressed: " << key << "with scancode " << scancode << " with action " << action << " with mods " << mods << std::endl;
    // Close window when Q is pressed
    if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        signalSIGINTHandler(2);
    }
    // Pause simulation
    else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        instance->shouldPause = !instance->shouldPause;
        std::cout << "Simulation " << (instance->shouldPause ? "paused" : "resumed") << std::endl;
    }
    // Switch render boundaries
    else if (key == GLFW_KEY_F2 && action == GLFW_RELEASE) {
        instance->renderBoundaries = !instance->renderBoundaries;
        std::cout << "Render boundaries: " << (instance->renderBoundaries ? "ON" : "OFF") << std::endl;
    }
    // Switch debug mode
    else if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
        instance->debugMode = !instance->debugMode;
        std::cout << "Debug mode: " << (instance->debugMode ? "ON" : "OFF") << std::endl;
    }
}

// Initialize and create window
int Visualizer::createWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    window = glfwCreateWindow(windowSize, windowSize, "QuadTree Visualizer", nullptr, nullptr);
    window_GEN = window;
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(NULL);

    windowThread = new std::thread([this]() {
        // Create window
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            return;
        }

        // Load font texture
        //loadFont("fonts");
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, windowSize, -1, windowSize+1, -1, 1);
        // I want to lock aspect ratio
        glfwSetWindowAspectRatio(window, 1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Capture CTRL+C to stop gracefully
        signal(SIGINT, signalSIGINTHandler);

        // Set the key callback
        glfwSetKeyCallback(window, keyboardCallback);

        while (!glfwWindowShouldClose(window) && !shouldClose) {
            glClear(GL_COLOR_BUFFER_BIT);
            displayCallback();
            glfwSwapBuffers(window);
            glfwPollEvents();
            // We sleep for 100ms
            usleep(100000);
        }

        // We mark the window as closed
        shouldClose = true;

        // Remove context from current thread
        glfwMakeContextCurrent(NULL);
        glfwDestroyWindow(window);
        glfwTerminate();

        std::cout << "Window closed" << std::endl;
    });

    return 0;
}

bool Visualizer::hasToClose() const {
    return shouldClose;
}

bool Visualizer::isInPause() const {
    return shouldPause;
}

bool Visualizer::isInDebug() {
    return debugMode;
}

bool* Visualizer::ptrDebugMode() {
    return &debugMode;
}

bool Visualizer::setPause(bool pause) {
    shouldPause = pause;
    return shouldPause;
}

bool Visualizer::setDebug(bool debug) {
    debugMode = debug;
    return debugMode;
}

void Visualizer::closeWindow() {
    shouldClose = true;
}

Visualizer* Visualizer::instance = nullptr;

Visualizer* Visualizer::getInstance(QuadTree* qt, double windowDefaultSize) {
    if (instance == nullptr) {
        instance = new Visualizer(qt, windowDefaultSize);
    } else {
        instance->qt = qt;
        instance->windowSize = windowDefaultSize;
    }
    
    return instance;
}

Visualizer* Visualizer::getInstance() {
    return instance;
}