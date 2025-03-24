#ifndef DRAWFONT_H
#define DRAWFONT_H

#ifndef VISU
#define VISU 1
#endif

#if VISU
#include <GL/glew.h>
#endif

#include <vector>
#include <string>

#if VISU
void loadFont(const char* folder);
GLuint loadTexture(const char* filepath);
void drawText(const std::string& text, float x, float y, float scale);
#endif

#endif // DRAWFONT_H