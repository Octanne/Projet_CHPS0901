#include "visualizer.h"

#include <unistd.h>
#include <iostream>
#include <csignal>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

#if VISU
#include "drawFont.h"
#endif

// Set debug mode to false
bool Visualizer::debugMode = false;
bool Visualizer::windowDebugMode = false;
// Set key pressed to false
bool Visualizer::keyLeftPressed = false;
bool Visualizer::keyRightPressed = false;
bool Visualizer::keyUpPressed = false;
bool Visualizer::keyDownPressed = false;
bool Visualizer::keyMinusPressed = false;
bool Visualizer::keyPlusPressed = false;

Visualizer::Visualizer(QuadTree* qt, double windowDefaultSize) {
    this->qt = qt;
    this->windowSize = windowDefaultSize;
    this->shouldClose = false;
    this->shouldPause = false;
    this->renderBoundaries = false;
    this->windowThread = nullptr;
    #if VISU
    this->window = nullptr;
    #endif

    this->winX = 0;
    this->winY = 0;
    this->scaleFactor = 1;
    this->fullRender = false;

    this->waitSem = false;
    this->accountedSem = false;
}
#if VISU
int Visualizer::drawQuadTreeArea(QuadTree* qt, int root) {
    if (qt == nullptr) {
        if (debugMode) std::cout << "Quad is null" << std::endl;
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
    double posX = (qt->getOriginX())*scaleFactorLocal+winX;
    double posY = (qt->getOriginY())*scaleFactorLocal+winY;
    // To draw at the correct pos in the window we have to substract the window position

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
        // Draw particle // We have to substract the window position
        double posXP = (qt->getParticle()->getX())*scaleFactorLocal+winX;
        double posYP = (qt->getParticle()->getY())*scaleFactorLocal+winY;
        
        // If point is in the window we draw it in green
        if (posXP >= -windowSize && posYP >= -windowSize && posXP <= windowSize && posYP <= windowSize) {
            //printf("Drawing particle at (%f, %f, %f)\n", qt->getX(), qt->getY(), qt->getMass());       
            glPointSize(3.0);
            glBegin(GL_POINTS);
            glColor3f(0.0, 1.0, 0.0);
            glVertex2f(posXP, posYP);
            glEnd();
            glEnd();
            return nbPart+1;
        }
    }

    if (root && debugMode) std::cout << "We printed " << nbPart << " particles" << std::endl;
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
    /*if (debugMode) std::cout << "Scale factor: " << scaleFactorLocal << " Width (scaled): " << width << "Width :" << qt->getWidth() << 
        "Factor from root :" << (qt->getWidth()/this->qt->getWidth()) <<
        " Origin :" << qt->getOriginX() << "," << qt->getOriginY() << std::endl;*/

    // Draw boundaries
    if (renderBoundaries) {
        double posX = qt->getOriginX()*scaleFactorLocal;
        double posY = qt->getOriginY()*scaleFactorLocal;
        //if (debugMode) std::cout << "Box origin at (" << posX << ", " << posY << ")" << std::endl;
        //printf("Drawing boundaries at (%f, %f, %f, %f)\n", posX-width, posY+width, posX+width, posY-width);
        glColor3f(0.0, 0.0, 1.0);
        glBegin(GL_LINE_LOOP);
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
        glPointSize(3.0);
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_POINTS);
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
    // color
    glColor3f(1.0, 1.0, 1.0);
    float size = 0.25;
    // We print the alphabet for test
    //std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    //drawText(alphabet, -windowSize + 10, windowSize - 35, 0.5);
    double topX = -windowSize/2 + 10;
    double topY = windowSize/2;
    std::stringstream stream;
    std::string text = "Debug mode : " + std::string(instance->debugMode ? "ON" : "OFF");
    drawText(text, topX, topY - 17.5, size);
    text = "Render boundaries : " + std::string(instance->renderBoundaries ? "ON" : "OFF");
    drawText(text, topX, topY - 35, size);
    text =  "Render mode : " + std::string(instance->fullRender ? "FULL" : "PARTIAL");
    drawText(text, topX, topY - 52.5, size);
    text =  "Window coordinates : (" + std::to_string(instance->winX) + ", " + std::to_string(instance->winY) + ")";
    drawText(text, topX, topY - 70, size);
    // We print the windowSize with only 2 decimals and use , as decimal separator
    stream.str("");
    stream << std::fixed << std::setprecision(2) << std::setfill('0') << std::setw(2) << std::right << std::setfill('0') << std::setw(2) << std::right << windowSize;
    text = "Window size : " + stream.str();
    drawText(text, topX, topY - 87.5, size);
    // We print the scaleFactor with only 2 decimals and use a comma as decimal separator
    stream.str("");
    stream << std::fixed << std::setprecision(2) << std::setfill('0') << std::setw(2) << std::right << std::setfill('0') << std::setw(2) << std::right << instance->scaleFactor;
    text = "Scale factor : " + stream.str();
    drawText(text, topX, topY - 105, size);
    text =  "Simulation : " + std::string(instance->shouldPause ? "PAUSE" : "RESUME");
    drawText(text, topX, topY - 122.5, size);
    text =  "Close : " + std::string(instance->shouldClose ? "YES" : "NO");
    drawText(text, topX, topY - 140, size);

    // We print a cross of 1 percent of the window size to mark the center
    double crossSize = windowSize * 0.01;
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-crossSize, 0);
    glVertex2f(crossSize, 0);
    glVertex2f(0, -crossSize);
    glVertex2f(0, crossSize);
    glEnd();
}

// Display callback function
void Visualizer::displayCallback() {
    if (qt != nullptr /*&& semCheck()*/) {
        glClear(GL_COLOR_BUFFER_BIT);
        if (fullRender) drawQuadTree(qt, 1);
        else drawQuadTreeArea(qt, 1);
        if (windowDebugMode) displayDebugDataInWindow();
        glEnd();
        // Usleep for 60 fps
        usleep(1000000/60);
    }
}

void Visualizer::waitClosedWindow() {
    windowThread->join();
}

void Visualizer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    instance->windowSize = width;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double ratio = (width/2)+1;
    glOrtho(-ratio, ratio, -ratio, ratio, -1, 1);
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
    if (action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT) keyLeftPressed = true;
        if (key == GLFW_KEY_RIGHT) keyRightPressed = true;
        if (key == GLFW_KEY_UP) keyUpPressed = true;
        if (key == GLFW_KEY_DOWN) keyDownPressed = true;
        if (key == GLFW_KEY_KP_ADD) keyPlusPressed = true;
        if (key == GLFW_KEY_KP_SUBTRACT) keyMinusPressed = true;
    }
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) keyLeftPressed = true;
        if (key == GLFW_KEY_RIGHT) keyRightPressed = true;
        if (key == GLFW_KEY_UP) keyUpPressed = true;
        if (key == GLFW_KEY_DOWN) keyDownPressed = true;
        if (key == GLFW_KEY_KP_ADD) keyPlusPressed = true;
        if (key == GLFW_KEY_KP_SUBTRACT) keyMinusPressed = true;
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT) keyLeftPressed = false;
        if (key == GLFW_KEY_RIGHT) keyRightPressed = false;
        if (key == GLFW_KEY_UP) keyUpPressed = false;
        if (key == GLFW_KEY_DOWN) keyDownPressed = false;
        if (key == GLFW_KEY_KP_ADD) keyPlusPressed = false;
        if (key == GLFW_KEY_KP_SUBTRACT) keyMinusPressed = false;
    }
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
        // We switch window debug mode
        instance->windowDebugMode = !instance->windowDebugMode;
        std::cout << "Window debug mode: " << (instance->windowDebugMode ? "ON" : "OFF") << std::endl;
    }
    // Switch between full render and partial render
    else if (key == GLFW_KEY_F4 && action == GLFW_RELEASE) {
        instance->fullRender = !instance->fullRender;
        std::cout << "Render mode: " << (instance->fullRender ? "FULL" : "PARTIAL") << std::endl;
    }    
    // Reset the window position
    else if (key == GLFW_KEY_END && action == GLFW_RELEASE) {
        instance->winX = 0;
        instance->winY = 0;
        std::cout << "Window coordinates: (" << instance->winX << ", " << instance->winY << ")" << std::endl;
    }
    // Reset the window factor
    else if (key == GLFW_KEY_KP_DIVIDE && action == GLFW_RELEASE) {
        instance->scaleFactor = 1;
        std::cout << "Scale factor: " << instance->scaleFactor << std::endl;
    }
    // Increase / Decrease the factor
    else if (keyPlusPressed) {
        instance->scaleFactor += 0.05;
        std::cout << "Scale factor: " << instance->scaleFactor << std::endl;
    }
    else if (keyMinusPressed) {
        instance->scaleFactor -= 0.05;
        std::cout << "Scale factor: " << instance->scaleFactor << std::endl;
    }
    // Move the window center
    else if (keyLeftPressed) {
        instance->winX -= 10;
        std::cout << "Window coordinates: (" << instance->winX << ", " << instance->winY << ")" << std::endl;
    }
    else if (keyRightPressed) {
        instance->winX += 10;
        std::cout << "Window coordinates: (" << instance->winX << ", " << instance->winY << ")" << std::endl;
    }
    else if (keyUpPressed) {
        instance->winY += 10;
        std::cout << "Window coordinates: (" << instance->winX << ", " << instance->winY << ")" << std::endl;
    }
    else if (keyDownPressed) {
        instance->winY -= 10;
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
        double ratio = (windowSize/2)+1;
        glOrtho(-ratio, ratio, -ratio, ratio, -1, 1);
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
#endif
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

bool Visualizer::semLock() {
    if (waitSem) {
        if (debugMode) std::cout << "Semaphore already locked" << std::endl;
        return false;
    } else {
        waitSem = true;
        accountedSem = false;
        if (debugMode) std::cout << "Semaphore awaiting accountabilty" << std::endl;
        // We wait for the semaphore to be accounted
        while (!accountedSem) {
            usleep(1000);
        }
        if (debugMode) std::cout << "Semaphore locked" << std::endl;
        return true;
    }
}

bool Visualizer::semCheck() {
    if (waitSem) {
        if (debugMode) std::cout << "Semaphore is currently locked" << std::endl;
        accountedSem = true;
        return false;
    } else {
        if (debugMode) std::cout << "Semaphore is currently unlocked" << std::endl;
        return true;
    }
}

bool Visualizer::semUnlock() {
    if (waitSem) {
        waitSem = false;
        accountedSem = false;
        if (debugMode) std::cout << "Semaphore unlocked" << std::endl;
        // We sleep for 1ms to let the other thread to lock the semaphore
        usleep(1000);
        return true;
    }
    if (debugMode) std::cout << "Semaphore already unlocked" << std::endl;
    return false;
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