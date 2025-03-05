#ifndef DRAWFONT_H
#define DRAWFONT_H

#include <GL/glew.h>

#include <vector>
#include <string>

extern GLuint fontTexture;
//std::vector<std::pair<float, float>> charUVs(256);  // Stores UVs for characters
//std::vector<std::pair<float, float>> charSizes(256);  // Stores sizes for characters

void loadFont(const char* folder);
GLuint loadTexture(const char* filepath);
void drawText(const std::string& text, float x, float y, float scale);

#endif // DRAWFONT_H