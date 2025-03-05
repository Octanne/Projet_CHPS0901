#include "visualizer.h"

#include <unistd.h>
#include <iostream>
#include <csignal>
#include <fstream>
#include <vector>

#include "drawFont.h"

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

    this->winX = 0;
    this->winY = 0;
    this->scaleFactor = 1;
    this->fullRender = true;
    this->windowDebugMode = false;
}

int Visualizer::drawQuadTreeArea(QuadTree* qt, int root) {
    if (qt == nullptr) {
        std::cout << "Quad is null" << std::endl;
        return 0;
    }
    int nbPart = 0;

    // I want when scaleFactor == 1 then computeFactor = windowSize/this->qt->getWidth()
    // If scaleFactor == 2 then computeFactor = windowSize/this->qt->getWidth()*0.5
    // If scaleFactor == 0.5 then computeFactor = windowSize/this->qt->getWidth()*2
    // So computeFactor = windowSize/(this->qt->getWidth()*scaleFactor)
    double scaleFactorLocal = windowSize/(this->qt->getWidth()*(1/scaleFactor));
    double width = qt->getWidth()*scaleFactorLocal*0.5;

    // Print scale factor and width
    if (root && debugMode) std::cout << "Scale factor: " << scaleFactor << " Width: " << width << std::endl;

    // We compute the position of the quadtree
    double posX = qt->getOriginX()*scaleFactorLocal;
    double posY = qt->getOriginY()*scaleFactorLocal;
    // To draw at the correct pos in the window we have to substract the window position
    posX += winX;
    posY += winY;

    // Draw boundaries
    if (renderBoundaries && posX-width < windowSize && posX+width > -windowSize && posY+width > -windowSize && posY-width < windowSize) {
        // If draw the quadtree only if the quadtree intersects the window
        //printf("Drawing boundaries at (%f, %f, %f, %f)\n", posX-width, posY+width, posX+width, posY-width);
        glBegin(GL_LINE_LOOP);
        glColor3f(0.0, 0.0, 1.0);
        glVertex2f(posX-width, posY+width);
        glVertex2f(posX+width, posY+width);
        glVertex2f(posX+width, posY-width);
        glVertex2f(posX-width, posY-width);
        glEnd();
    }

    // We draw the other quads
    if (qt->isItDivided()) {
        // Draw other Quads
        nbPart += drawQuadTreeArea(qt->getSouthwest());
        nbPart += drawQuadTreeArea(qt->getSoutheast());
        nbPart += drawQuadTreeArea(qt->getNorthwest());
        nbPart += drawQuadTreeArea(qt->getNortheast());
    } else if (qt->hasParticle()) {
        // Draw particle
        double posX = qt->getParticle()->getX()*scaleFactorLocal;
        double posY = qt->getParticle()->getY()*scaleFactorLocal;
        // We have to substract the window position
        posX += winX;
        posY += winY;
        // If point is in the window we draw it in green
        if (posX >= -windowSize && posY >= -windowSize && posX <= windowSize && posY <= windowSize) {
            //printf("Drawing particle at (%f, %f, %f)\n", qt->getX(), qt->getY(), qt->getMass());
            glPointSize(5.0);
            glBegin(GL_POINTS);
            glColor3f(0.0, 1.0, 0.0);
            glVertex2f(posX, posY);
            glEnd();
            return nbPart+1;
        }
    }

    if (root == 1 && debugMode) std::cout << "We printed " << nbPart << " particles" << std::endl;

    return nbPart;
}

// Draw quadtree but in full scalled to the window size
int Visualizer::drawQuadTree(QuadTree* qt, int root) {
    if (qt == nullptr) {
        std::cout << "Quad is null" << std::endl;
        return 0;
    }
    int nbPart = 0;
    double scaleFactorLocal = windowSize/this->qt->getWidth();
    double width = qt->getWidth()*scaleFactorLocal*0.5;

    // Print scale factor and width
    if (root && debugMode) std::cout << "Scale factor: " << scaleFactorLocal << " Width: " << width << std::endl;

    // Draw boundaries
    if (renderBoundaries) {
        double posX = qt->getOriginX()*scaleFactorLocal;
        double posY = qt->getOriginY()*scaleFactorLocal;
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
        nbPart += drawQuadTree(qt->getSouthwest());
        nbPart += drawQuadTree(qt->getSoutheast());
        nbPart += drawQuadTree(qt->getNorthwest());
        nbPart += drawQuadTree(qt->getNortheast());
    } else if (qt->hasParticle()) {
        // Draw particle
        //printf("Drawing particle at (%f, %f, %f)\n", qt->getX(), qt->getY(), qt->getMass());
        glPointSize(5.0);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);
        double posX = qt->getParticle()->getX()*scaleFactorLocal;
        double posY = qt->getParticle()->getY()*scaleFactorLocal;
        glVertex2f(posX, posY);
        glEnd();
        return nbPart+1;
    }

    if (root == 1 && debugMode) std::cout << "We printed " << nbPart << " particles" << std::endl;

    return nbPart;
}

void Visualizer::displayDebugDataInWindow() {
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(10, windowSize - 20);
    std::string text = "shouldClose: " + std::to_string(shouldClose);
    drawText(text, 10, windowSize - 20, 2.0);
    glRasterPos2f(10, windowSize - 40);
    text = "shouldPause: " + std::to_string(shouldPause);
    drawText(text, 10, windowSize - 40, 2.0);
    glRasterPos2f(10, windowSize - 60);
    text = "renderBoundaries: " + std::to_string(renderBoundaries);
    drawText(text, 10, windowSize - 60, 2.0);
    glRasterPos2f(10, windowSize - 80);
    text = "debugMode: " + std::to_string(debugMode);
    drawText(text, 10, windowSize - 80, 2.0);
    glRasterPos2f(10, windowSize - 100);
    text = "windowSize: " + std::to_string(windowSize);
    drawText(text, 10, windowSize - 100, 2.0);
}

// Display callback function
void Visualizer::displayCallback() {
    if (qt != nullptr) {
        if (fullRender) drawQuadTree(qt, 1);
        else drawQuadTreeArea(qt, 1);
        // Usleep for 60 fps
        usleep(1000000/60);
        if (debugMode) displayDebugDataInWindow();
    }
}

void Visualizer::waitClosedWindow() {
    windowThread->join();
}

void Visualizer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    instance->windowSize = width;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-instance->windowSize, instance->windowSize, -instance->windowSize, instance->windowSize, -1, 1);
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
    // Switch debug mode
    else if (key == GLFW_KEY_F1 && action == GLFW_RELEASE) {
        instance->debugMode = !instance->debugMode;
        std::cout << "Debug mode: " << (instance->debugMode ? "ON" : "OFF") << std::endl;
    }        
    // Switch render boundaries
    else if (key == GLFW_KEY_F2 && action == GLFW_RELEASE) {
        instance->renderBoundaries = !instance->renderBoundaries;
        std::cout << "Render boundaries: " << (instance->renderBoundaries ? "ON" : "OFF") << std::endl;
    }
    // Print the options status
    else if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
        std::cout << "Debug mode: " << (instance->debugMode ? "ON" : "OFF") << std::endl;
        std::cout << "Render boundaries: " << (instance->renderBoundaries ? "ON" : "OFF") << std::endl;
        std::cout << "Render mode: " << (instance->fullRender ? "FULL" : "PARTIAL") << std::endl;
        std::cout << "Window coordinates: (" << instance->winX << ", " << instance->winY << ")" << std::endl;
        std::cout << "Window size: " << instance->windowSize << std::endl;
        std::cout << "Scale factor: " << instance->scaleFactor << std::endl;
        std::cout << "Pause: " << (instance->shouldPause ? "ON" : "OFF") << std::endl;
        std::cout << "Close: " << (instance->shouldClose ? "ON" : "OFF") << std::endl;
    }
    // Switch between full render and partial render
    else if (key == GLFW_KEY_F4 && action == GLFW_RELEASE) {
        instance->fullRender = !instance->fullRender;
        std::cout << "Render mode: " << (instance->fullRender ? "FULL" : "PARTIAL") << std::endl;
    }

    // Increase the factor of the window
    else if (key == GLFW_KEY_KP_ADD && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        instance->scaleFactor *= 1.1;
        std::cout << "Scale factor: " << instance->scaleFactor << std::endl;
    }
    // Decrease the factor of the window
    else if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        instance->scaleFactor /= 1.1;
        std::cout << "Scale factor: " << instance->scaleFactor << std::endl;
    }
    // Reset the factor of the window
    else if (key == GLFW_KEY_KP_MULTIPLY && action == GLFW_RELEASE) {
        instance->scaleFactor = 1;
        std::cout << "Scale factor: " << instance->scaleFactor << std::endl;
    }

    // Move the window to the right
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        instance->winX -= 10;
        std::cout << "Window coordinates: (" << instance->winX << ", " << instance->winY << ")" << std::endl;
    }
    // Move the window to the left
    else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        instance->winX += 10;
        std::cout << "Window coordinates: (" << instance->winX << ", " << instance->winY << ")" << std::endl;
    }
    // Move the window to the top
    else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        instance->winY += 10;
        std::cout << "Window coordinates: (" << instance->winX << ", " << instance->winY << ")" << std::endl;
    }
    // Move the window to the bottom
    else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        instance->winY -= 10;
        std::cout << "Window coordinates: (" << instance->winX << ", " << instance->winY << ")" << std::endl;
    }
    // Reset the window position
    else if (key == GLFW_KEY_END && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        instance->winX = 0;
        instance->winY = 0;
        std::cout << "Window coordinates: (" << instance->winX << ", " << instance->winY << ")" << std::endl;
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
        loadFont("fonts");
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-windowSize, windowSize, -windowSize, windowSize, -1, 1);
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