#include "visualizer.h"

#include <iostream>
#include <unistd.h>
#include <thread>
#include <csignal>
#include <fstream>
#include <vector>

// install sudo apt install libglew-dev libglfw3-dev libglm-dev
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Global QuadTree pointer for display callback
std::thread* windowThread = nullptr;
bool shouldClose = false;
bool shouldPause = false;
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

GLuint fontTexture;
std::vector<std::pair<float, float>> charUVs(256);  // Stores UVs for characters
std::vector<std::pair<float, float>> charSizes(256);  // Stores sizes for characters

GLuint loadTexture(const char* filepath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    return textureID;
}

void drawText(const std::string& text, float x, float y, float scale) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0f);

    glBegin(GL_QUADS);
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        float u = charUVs[c].first;
        float v = charUVs[c].second;
        float w = charSizes[c].first;
        float h = charSizes[c].second;

        glTexCoord2f(u, v);
        glVertex2f(0, 0);
        glTexCoord2f(u + w, v);
        glVertex2f(w, 0);
        glTexCoord2f(u + w, v + h);
        glVertex2f(w, h);
        glTexCoord2f(u, v + h);
        glVertex2f(0, h);

        glTranslatef(w, 0, 0); // Move to the next character position
    }
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void loadFont(const char* folder) {
    std::string fontFile = std::string(folder) + "/font.fnt";
    std::string textureFile = std::string(folder) + "/font_0.tga";

    fontTexture = loadTexture(textureFile.c_str());
    if (!fontTexture) return;

    std::ifstream file(fontFile);
    if (!file) {
        std::cerr << "Failed to open font file: " << fontFile << std::endl;
        return;
    }

    // Assuming texture dimensions are 256x256 based on .fnt file information
    const float textureWidth = 256.0f;
    const float textureHeight = 256.0f;

    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 5) == "char ") {
            int id, x, y, width, height, xoffset, yoffset, xadvance;
            sscanf(line.c_str(), "char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d",
                   &id, &x, &y, &width, &height, &xoffset, &yoffset, &xadvance);

            // Calculate normalized UV coordinates
            float u1 = x / textureWidth;
            float v1 = y / textureHeight;
            float u2 = (x + width) / textureWidth;
            float v2 = (y + height) / textureHeight;

            // Store the UVs for the character
            charUVs[id] = std::make_pair(u1, v1);
            charSizes[id] = std::make_pair(u2 - u1, v2 - v1);  // store the width and height as UV deltas
        }
    }

    file.close();
}

void displayDebugDataInWindow() {
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(10, (*windowSize) - 20);
    std::string text = "shouldClose: " + std::to_string(shouldClose);
    drawText(text, 10, (*windowSize) - 20, 1.0);
  
    glRasterPos2f(10, (*windowSize) - 40);
    text = "shouldPause: " + std::to_string(shouldPause);
    drawText(text, 10, (*windowSize) - 40, 1.0);

    glRasterPos2f(10, (*windowSize) - 60);
    text = "renderBoundaries: " + std::to_string(renderBoundaries);
    drawText(text, 10, (*windowSize) - 60, 1.0);

    glRasterPos2f(10, (*windowSize) - 80);
    text = "debugMode: " + std::to_string(debugMode);
    drawText(text, 10, (*windowSize) - 80, 1.0);

    glRasterPos2f(10, (*windowSize) - 100);
    text = "windowSize: " + std::to_string(*windowSize);
    drawText(text, 10, (*windowSize) - 100, 1.0);
}

// Display callback function
void displayCallback(QuadTree* qt, double simulationSize) {
    if (qt != nullptr) {
        drawQuadTree(qt, simulationSize, 1);
        // Usleep for 60 fps
        usleep(1000000/60);
        //if (debugMode) displayDebugDataInWindow();
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

        // Load font texture
        loadFont("fonts");
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
            // Pause simulation
            else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
                shouldPause = !shouldPause;
                std::cout << "Simulation " << (shouldPause ? "paused" : "resumed") << std::endl;
            }
            // Switch render boundaries
            else if (key == GLFW_KEY_F2 && action == GLFW_RELEASE) {
                renderBoundaries = !renderBoundaries;
                std::cout << "Render boundaries: " << (renderBoundaries ? "ON" : "OFF") << std::endl;
            }
            // Switch debug mode
            else if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
                debugMode = !debugMode;
                std::cout << "Debug mode: " << (debugMode ? "ON" : "OFF") << std::endl;
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
