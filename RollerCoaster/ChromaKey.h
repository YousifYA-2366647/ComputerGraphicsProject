#ifndef CHROMA_KEY_H
#define CHROMA_KEY_H

#include <glad/glad.h>
#include "Shader.h"
#include <string>

class ChromaKeyPictureFrame {
public:
    ChromaKeyPictureFrame(const std::string& imagePath);
    ~ChromaKeyPictureFrame();
    void Draw();

private:
    unsigned int VAO, VBO, texture; // texture id for image
    Shader* shader;
    void setupQuad();// Setup quad for image
    void loadTexture(const std::string& path);
};

#endif