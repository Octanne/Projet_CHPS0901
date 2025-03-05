#ifndef DRAWFONT_H
#define DRAWFONT_H

#include <GL/glew.h>

#include <vector>
#include <string>

void loadFont(const char* folder);
GLuint loadTexture(const char* filepath);
void drawText(const std::string& text, float x, float y, float scale);

#endif // DRAWFONT_H