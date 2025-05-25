#include "Convolution.h"

Convolution::Convolution(unsigned int screenWidth, unsigned int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight) {
    quadVertices = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    edgeShader = new Shader("screenVertexShader.vert", "edge.frag");
    blurShader = new Shader("screenVertexShader.vert", "blur.frag");
    invertShader = new Shader("screenVertexShader.vert", "invert.frag");
    grayShader = new Shader("screenVertexShader.vert", "grayScale.frag");
    sharpenShader = new Shader("screenVertexShader.vert", "sharpen.frag");
    bloomExtractShader = new Shader("screenVertexShader.vert", "bloomExtract.frag");
    bloomBlurShader = new Shader("screenVertexShader.vert", "bloomBlur.frag");
    bloomCombineShader = new Shader("screenVertexShader.vert", "bloomCombine.frag");

    setupConvolution();
    setupPingpongBuffers();
}

Convolution::~Convolution() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteTextures(1, &textureColorBuffer);
    glDeleteFramebuffers(1, &hdrFBO);
    glDeleteTextures(2, colorBuffers);
    glDeleteFramebuffers(2, pingpongFBO);
    glDeleteTextures(2, pingpongColorbuffers);
    delete edgeShader;
    delete blurShader;
    delete invertShader;
    delete grayShader;
    delete sharpenShader;
    delete bloomExtractShader;
    delete bloomBlurShader;
    delete bloomCombineShader;
}

void Convolution::setupConvolution() {
    // === CREATE REGULAR FBO FIRST ===
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

    // === CREATE HDR FBO FOR BLOOM ===
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    // Tell OpenGL which color attachments we'll use for rendering
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    // Create depth renderbuffer for HDR framebuffer
    glGenRenderbuffers(1, &hdrRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, hdrRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, hdrRBO);

    // Check if HDR framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: HDR Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

    // Setup shader uniforms
    bloomExtractShader->use();
    bloomExtractShader->setInt("screenTexture", 0);
    bloomBlurShader->use();
    bloomBlurShader->setInt("image", 0);
    bloomCombineShader->use();
    bloomCombineShader->setInt("scene", 0);
    bloomCombineShader->setInt("bloomBlur", 1);
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

    // Set effect intensity
    shader->use();
    shader->setInt("screenTexture", 0);
    shader->setFloat("offset", intensity / screenWidth);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Convolution::bindBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void Convolution::bindHDR() {
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // Make sure we're writing to both color attachments
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
}

void Convolution::resize(unsigned int newWidth, unsigned int newHeight) {
    if (newWidth == screenWidth && newHeight == screenHeight) return;

    screenWidth = newWidth;
    screenHeight = newHeight;

    // Clean up old resources
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (FBO) glDeleteFramebuffers(1, &FBO);
    if (textureColorBuffer) glDeleteTextures(1, &textureColorBuffer);
    if (RBO) glDeleteRenderbuffers(1, &RBO);
    if (hdrFBO) glDeleteFramebuffers(1, &hdrFBO);
    if (colorBuffers[0]) glDeleteTextures(2, colorBuffers);
    if (hdrRBO) glDeleteRenderbuffers(1, &hdrRBO);
    if (pingpongFBO[0]) glDeleteFramebuffers(2, pingpongFBO);
    if (pingpongColorbuffers[0]) glDeleteTextures(2, pingpongColorbuffers);

    // Reload resources
    setupConvolution();
    setupPingpongBuffers();
}

void Convolution::setupPingpongBuffers() {
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR::FRAMEBUFFER:: Pingpong framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Convolution::Bloom(float threshold, float blurIntensity, int blurPasses) {
    // Disable depth testing for post-processing
    glDisable(GL_DEPTH_TEST);

    // Extract bright parts from the scene
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
    glViewport(0, 0, screenWidth, screenHeight);
    glClear(GL_COLOR_BUFFER_BIT);

    bloomExtractShader->use();
    bloomExtractShader->setInt("screenTexture", 0);
    bloomExtractShader->setFloat("threshold", threshold);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Blur bright parts using ping-pong technique
    bool horizontal = true;
    bool firstIteration = true;
    bloomBlurShader->use();

    for (int i = 0; i < blurPasses; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        glClear(GL_COLOR_BUFFER_BIT);

        // Progressive blur - each pass gets stronger
        // Better for performance
        float progressiveBlur = blurIntensity * (1.0f + i * 0.5f);

        bloomBlurShader->setInt("horizontal", horizontal ? 1 : 0);
        bloomBlurShader->setFloat("offset", progressiveBlur / (horizontal ? screenWidth : screenHeight));
        bloomBlurShader->setInt("image", 0);
        bloomBlurShader->setFloat("blurRadius", progressiveBlur);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, firstIteration ? pingpongColorbuffers[0] : pingpongColorbuffers[!horizontal]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        horizontal = !horizontal;
        if (firstIteration)
            firstIteration = false;
    }

    // Combine original scene with blurred bloom
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenWidth, screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bloomCombineShader->use();
    bloomCombineShader->setInt("scene", 0);
    bloomCombineShader->setInt("bloomBlur", 1);
    bloomCombineShader->setFloat("exposure", 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}