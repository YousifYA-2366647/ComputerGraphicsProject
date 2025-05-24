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
    unsigned int VAO, VBO, texture;
    Shader* shader;
    void setupQuad();
    void loadTexture(const std::string& path);
};

#endif