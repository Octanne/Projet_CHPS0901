#include "drawFont.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <fstream>

// We set fontTexture to 0
GLuint fontTexture = 0;
// We set charUVs and charSizes to 256
std::vector<std::pair<float, float>> charUVs(256);
std::vector<std::pair<float, float>> charSizes(256);

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