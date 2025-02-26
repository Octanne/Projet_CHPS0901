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

// Recursively draw the QuadTree
int drawQuadTree(QuadTree* qt, int width, int posX, int posY, int root = 0) {
    if (qt == nullptr) {
        std::cout << "Quad is null" << std::endl;
        return 0;
    }
    int nbPart = 0;

    // TODO : Draw boundaries
    //printf("Drawing boundaries at (%d, %d, %d, %d)\n", posX, posY, posX+width, posY+width);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(posX, posY);
    glVertex2f(posX+width, posY);
    glVertex2f(posX+width, posY+width);
    glVertex2f(posX, posY+width);
    glEnd();

    if (qt->isItDivided()) {
        // Draw other Quads
        int newWidth = width/2;
        nbPart += drawQuadTree(qt->getSouthwest(), newWidth, posX, posY);
        nbPart += drawQuadTree(qt->getSoutheast(), newWidth, posX+newWidth, posY);
        nbPart += drawQuadTree(qt->getNorthwest(), newWidth, posX, posY+newWidth);
        nbPart += drawQuadTree(qt->getNortheast(), newWidth, posX+newWidth, posY+newWidth);
    } else if (qt->hasParticle()) {
        // Draw particle
        //printf("Drawing particle at (%f, %f, %f)\n", qt->getX(), qt->getY(), qt->getMass());
        glPointSize(3.0);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(qt->getParticle()->getX(), qt->getParticle()->getY());
        glEnd();
        return nbPart+1;
    }

    if (root) {
        //std::cout << "We printed " << nbPart << " particles" << std::endl;
    }

    return nbPart;
}

// Display callback function
void displayCallback(QuadTree* qt, int globalWidth) {
    if (qt != nullptr) {
        drawQuadTree(qt, globalWidth, 0, 0, 1);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    std::cout << "Resizing of window id " << glfwGetWindowAttrib(window, GLFW_FOCUSED) << " to " << width << "x" << height << std::endl;
}

// Wait for the window to be closed
void waitClosedWindow() {
    windowThread->join();
}

GLFWwindow* window_GEN;
void signalHandler(int signum) {
    glfwSetWindowShouldClose(window_GEN, GLFW_TRUE);
    std::cout << "Interrupt signal (" << signum << ") received. Closing window" << std::endl;
}

// Initialize and create window
int createWindow(QuadTree* qt, int width) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(width, width, "QuadTree Visualizer", nullptr, nullptr);
    window_GEN = window;
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(NULL);

    windowThread = new std::thread([window, qt, width] {
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
        glOrtho(-1, width, -1, width+1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Capture CTRL+C to stop gracefully
        signal(SIGINT, signalHandler);

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);
            displayCallback(qt, width);
            glfwSwapBuffers(window);
            glfwPollEvents();
            // We sleep for 100ms
            usleep(100000);
        }

        // Remove context from current thread
        glfwMakeContextCurrent(NULL);
        glfwDestroyWindow(window);
        glfwTerminate();

        std::cout << "Window closed" << std::endl;
    });

    return 0;
}
