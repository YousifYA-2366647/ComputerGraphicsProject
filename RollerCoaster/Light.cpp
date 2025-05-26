#include "Light.h"
#include <glm-1.0.1/glm/gtc/matrix_transform.hpp>

LightManager::LightManager() {
    lightShader = nullptr;
    
}
//use shader 
void LightManager::initialize() {
    if (!lightShader) {
        lightShader = new Shader("lightShader.vert", "lightShader.frag");
        setupLights();
    }
}
LightManager::~LightManager() {
    delete lightShader;
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
}
// lights adding -> manager
void LightManager::addLight(const Light& light) {
    lights.push_back(light);
}

void LightManager::setupLights() {
    
    float vertices[] = {
        // posities          
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void LightManager::drawLights(const glm::mat4& view, const glm::mat4& projection) {
    if (!lightShader) {
        return;
    }
    
    lightShader->use();
    lightShader->setMat4("view", view);
    lightShader->setMat4("projection", projection);
    
    glBindVertexArray(lightVAO);
    
    for (const auto& light : lights) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light.position);
        model = glm::scale(model, glm::vec3(0.2f)); 
        
        lightShader->setMat4("model", model);
        lightShader->setVec3("lightColor", light.color);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
// Apply lighting to the shader
void LightManager::applyLighting(Shader* targetShader, const glm::vec3& viewPos) {
    targetShader->use();
    targetShader->setVec3("viewPos", viewPos);

    targetShader->setInt("numLights", lights.size());
    for (int i = 0; i < lights.size(); i++) {
        std::string prefix = "lights[" + std::to_string(i) + "]";
        targetShader->setVec3(prefix + ".position", lights[i].position);
        targetShader->setVec3(prefix + ".color", lights[i].color);
        targetShader->setFloat(prefix + ".constant", lights[i].constant);
        targetShader->setFloat(prefix + ".linear", lights[i].linear);
        targetShader->setFloat(prefix + ".quadratic", lights[i].quadratic);
    }
}