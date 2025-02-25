#include "visualizer.h"

#include <iostream>
#include <unistd.h>
#include <thread>

// install sudo apt install libglew-dev libglfw3-dev libglm-dev
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Global QuadTree pointer for display callback
std::thread* windowThread = nullptr;
int globalWidth = 800;

// Recursively draw the QuadTree
int drawQuadTree(QuadTree* qt, int width, int root, int posX, int posY) {
    if (qt == nullptr) {
        std::cout << "Quad is null" << std::endl;
        return 0;
    }
    int nbPart = 0;

    // TODO : Draw boundaries
    //printf("Drawing boundaries at (%d, %d, %d, %d)\n", posX, posY, posX+width, posY+width);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(posY, posX);
    glVertex2f(posY+width, posX);
    glVertex2f(posY+width, posX+width);
    glVertex2f(posY, posX+width);
    glEnd();

    if (qt->isItDivided()) {
        // Draw other Quads
        //std::cout << "Node is divided" << std::endl;
        nbPart += drawQuadTree(qt->getNortheast(), width/2, 0, posX+width/2, posY+width/2);
        nbPart += drawQuadTree(qt->getNorthwest(), width/2, 0, posX, posY+width/2);
        nbPart += drawQuadTree(qt->getSoutheast(), width/2, 0, posX+width/2, posY);
        nbPart += drawQuadTree(qt->getSouthwest(), width/2, 0, posX, posY);
    } else if (qt->hasParticle()) {
        // Draw particle
        //printf("Drawing particle at (%f, %f, %f)\n", qt->getX(), qt->getY(), qt->getMass());
        glPointSize(3.0);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(qt->getX(), qt->getY());
        glEnd();
        return nbPart+1;
    }

    if (root) {
        std::cout << "We printed " << nbPart << " particles" << std::endl;
    }

    return nbPart;
}

// Display callback function
void displayCallback(QuadTree* qt) {
    if (qt != nullptr) {
        drawQuadTree(qt, globalWidth, 1, 0, 0);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Wait for the window to be closed
void waitClosedWindow() {
    windowThread->join();
}

// Initialize and create window
int createWindow(QuadTree* qt) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(globalWidth, globalWidth, "QuadTree Visualizer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(NULL);

    windowThread = new std::thread([window, qt] {
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
        glOrtho(-globalWidth, globalWidth, -globalWidth, globalWidth, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);
            displayCallback(qt);

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
