#include "ChromaKey.h"
#include <iostream>
#include "stb_image.h"
//constrcutor
ChromaKeyPictureFrame::ChromaKeyPictureFrame(const std::string& imagePath) {
    shader = new Shader("chromaVertexShader.vert", "chromaKeyFragmentShader.frag");
    
    setupQuad();
    
    loadTexture(imagePath);
}
// destructor
ChromaKeyPictureFrame::~ChromaKeyPictureFrame() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
    delete shader;
}
// quad setup 
void ChromaKeyPictureFrame::setupQuad() {
    // Screen-aligned quad vertices
    float quadVertices[] = {
        
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f  
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}
// texture loading
void ChromaKeyPictureFrame::loadTexture(const std::string& path) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    
    if (data) {
        GLenum format;
        if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            stbi_image_free(data);
            return; }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "fail met het laden" << std::endl;
    } else {
        std::cout << "Failed laden " << path << std::endl;
    }
    
    stbi_image_free(data);
}
// Draw de chroma key overlay
void ChromaKeyPictureFrame::Draw() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //geen depth test nodig voor overlay
    glDisable(GL_DEPTH_TEST);
    
    shader->use();
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->setInt("overlayTexture", 0);
    
    // Teken de quad
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEnable(GL_DEPTH_TEST);
}