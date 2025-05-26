#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm-1.0.1/glm/glm.hpp>
#include <vector>
#include "Shader.h"

class Light {
public:
    glm::vec3 position;
    glm::vec3 color;
    float constant; // attenauatie constant
    float linear; // lineaire attenuatie
    float quadratic; // kwadratische attenuatie

    Light(glm::vec3 pos, glm::vec3 col, float cons, float lin, float quad)
        : position(pos), color(col), constant(cons), linear(lin), quadratic(quad) {
    }
};

class LightManager {
public:
    LightManager();
    ~LightManager();
    void initialize();
    void addLight(const Light& light);
    void setupLights();
    void drawLights(const glm::mat4& view, const glm::mat4& projection);
    void applyLighting(Shader* targetShader, const glm::vec3& viewPos);

    const std::vector<Light>& getLights() const { return lights; }

private:
    std::vector<Light> lights;
    unsigned int lightVAO, lightVBO;
    Shader* lightShader;
};

#endif