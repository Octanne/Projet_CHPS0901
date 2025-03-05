#include "drawFont.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <fstream>

// We set fontTexture to 0
GLuint fontTexture = 0;
// We set charUVs and charSizes to 256
std::vector<std::pair<float, float>> charUVs(256);
std::vector<std::pair<float, float>> charUVs2(256);
std::vector<std::pair<float, float>> charSizes(256);

GLuint loadTexture(const char* filepath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return 0;
    }

    std::cout << "Loaded texture: " << filepath << " with width: " << width << " and height: " << height << std::endl;

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
            float w = width;
            float h = height;

            // Store the UVs for the character
            charUVs[id] = std::make_pair(u1, v1);
            charUVs2[id] = std::make_pair(u2, v2);
            charSizes[id] = std::make_pair(w, h);
            //std::cout << "Character " << id << " which is the character " << (char)id << std::endl;
        }
    }

    // We reduce the size of the space character
    charSizes[' '].first = 6;

    file.close();
    std::cout << "Font loaded successfully" << std::endl;
}

void drawText(const std::string& text, float x, float y, float scale) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1);

    glBegin(GL_QUADS);
    // We print the text
    double xW = 0; double yW = 0;
    for (char c : text) {
        float u1 = charUVs[c].first;
        float v1 = charUVs[c].second;
        float u2 = charUVs2[c].first;
        float v2 = charUVs2[c].second;
        float w = charSizes[c].first;
        float h = charSizes[c].second;

        // We print the quad of the dimensions of the character with the correct UVs
        glTexCoord2f(u1, v2);
        glVertex2f(xW, yW);
        glTexCoord2f(u2, v2);
        glVertex2f(xW + w, yW);
        glTexCoord2f(u2, v1);
        glVertex2f(xW + w, yW + h);
        glTexCoord2f(u1, v1);
        glVertex2f(xW, yW + h);

        xW += w;
        //std::cout << "Character " << c << " is id " << (int)c << std::endl;
    }
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}