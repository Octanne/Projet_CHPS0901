#include "drawFont.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <fstream>

// We set fontTexture
std::vector<GLuint> fontTexture(256);
// We set charUVs and charSizes to 256
std::vector<std::pair<float, float>> charUVs(256);
std::vector<std::pair<float, float>> charUVs2(256);
std::vector<std::pair<float, float>> charSizes(256);
std::vector<std::pair<float, float>> charOffsets(256);
std::vector<int> charPages(256);

GLuint loadTexture(const char* filepath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return 0;
    }

    //std::cout << "Loaded texture: " << filepath << " with width: " << width << " and height: " << height << std::endl;

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

    std::ifstream file(fontFile);
    if (!file) {
        std::cerr << "Failed to open font file: " << fontFile << std::endl;
        return;
    }

    // Assuming texture dimensions are 256x256 based on .fnt file information
    float textureWidth = 256.0f;
    float textureHeight = 256.0f;

    std::string line;
    while (std::getline(file, line)) {
        // We get the size of the textures from the line common lineHeight=64 base=51 scaleW=256 scaleH=256
        if (line.substr(0, 7) == "common ") {
            sscanf(line.c_str(), "common lineHeight=%*d base=%*d scaleW=%f scaleH=%f", &textureWidth, &textureHeight);
            //std::cout << "Texture size: " << textureWidth << "x" << textureHeight << std::endl;
        }
        // We load the textures
        if (line.substr(0, 5) == "page ") {
            char textureName[256]; int id;
            sscanf(line.c_str(), "page id=%d file=\"%[^\"]\"", &id, textureName);
            // We add the folder to the texture name
            std::string fullTextureName = std::string(folder) + "/" + textureName;
            //std::cout << "Loading texture: " << fullTextureName << std::endl;
            GLuint textureID = loadTexture(fullTextureName.c_str());
            if (!textureID) return;
            fontTexture[id] = textureID;
        } 
        // We load the characters
        else
        if (line.substr(0, 5) == "char ") {
            int id, x, y, width, height, xoffset, yoffset, xadvance, page;
            sscanf(line.c_str(), "char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d page=%d", 
            &id, &x, &y, &width, &height, &xoffset, &yoffset, &xadvance, &page);

            // Calculate normalized UV coordinates
            float u1 = x / textureWidth;
            float v1 = y / textureHeight;
            float u2 = (x + width) / textureWidth;
            float v2 = (y + height) / textureHeight;
            float w = width;
            float h = height;
            float xoff = 0;
            float yoff = 0;
            //float yoff = -yoffset;

            // Store the UVs for the character
            charUVs[id] = std::make_pair(u1, v1);
            charUVs2[id] = std::make_pair(u2, v2);
            charSizes[id] = std::make_pair(w, h);
            charOffsets[id] = std::make_pair(xoff, yoff);
            charPages[id] = page;
            //std::cout << "Character " << id << " which is the character " << (char)id << std::endl;
        }
    }

    // We reduce the size of the space character
    charSizes[' '].first = charSizes['i'].first;

    file.close();
    std::cout << "Font loaded successfully" << std::endl;
}

void drawText(const std::string& text, float x, float y, float scale) {
    GLuint lastTexture = fontTexture[0];    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lastTexture);
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
        float xoffset = charOffsets[c].first;
        float yoffset = charOffsets[c].second;
        GLuint textureID = fontTexture[charPages[c]];

        // We bind the texture
        if (textureID != lastTexture) {
            // We switch to the new texture
            glEnd();
            glPopMatrix();
            glBindTexture(GL_TEXTURE_2D, textureID);
            glPushMatrix();
            glTranslatef(x, y, 0);
            glScalef(scale, scale, 1);
            glBegin(GL_QUADS);
            lastTexture = textureID;
        }
        
        // We print the quad of the dimensions of the character with the correct UVs
        glTexCoord2f(u1, v2);
        glVertex2f(xW + xoffset, yW + yoffset);
        glTexCoord2f(u2, v2);
        glVertex2f(xW + xoffset + w, yW + yoffset);
        glTexCoord2f(u2, v1);
        glVertex2f(xW + xoffset + w, yW + yoffset + h);
        glTexCoord2f(u1, v1);
        glVertex2f(xW + xoffset, yW + yoffset + h);

        xW += w;
        //std::cout << "Character " << c << " is id " << (int)c << std::endl;
    }
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}