#include "Convolution.h"

Convolution::Convolution(unsigned int screenWidth, unsigned int screenHeight): screenWidth(screenWidth), screenHeight(screenHeight) {
    quadVertices = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    setupConvolution();

    edgeShader = new Shader("screenVertexShader.vert", "edge.frag");
    blurShader = new Shader("screenVertexShader.vert", "blur.frag");
    invertShader = new Shader("screenVertexShader.vert", "invert.frag");
    grayShader = new Shader("screenVertexShader.vert", "grayScale.frag");
    sharpenShader = new Shader("screenVertexShader.vert", "sharpen.frag");
}

Convolution::~Convolution() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteTextures(1, &textureColorBuffer);
    delete edgeShader;
    delete blurShader;
    delete invertShader;
    delete grayShader;
    delete sharpenShader;
}


void Convolution::setupConvolution() {
    // === CREATE FBO FIRST ===
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Create a color attachment texture
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    // Create a renderbuffer object for depth and stencil
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind

    // Setup screen quad VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);

    // Vertex layout: position (2 floats), texCoord (2 floats)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Convolution::Blur(float intensity) {
    draw(blurShader, intensity);
}

void Convolution::EdgeDetect(float intensity) {
    draw(edgeShader, intensity);
}

void Convolution::Inverse(float intensity) {
    draw(invertShader, intensity);
}

void Convolution::GrayScale(float intensity) {
    draw(grayShader, intensity);
}

void Convolution::Sharpen(float intensity) {
    draw(sharpenShader, intensity);
}

void Convolution::draw(Shader* shader, float intensity) {
    if (!shader) return; // Safety check

    shader->use();
    shader->setInt("screenTexture", 0);
    shader->setFloat("offset", intensity / screenWidth); // Or screenHeight, depending

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Convolution::bindBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}