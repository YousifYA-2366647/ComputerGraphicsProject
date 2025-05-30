#ifndef CART_H
#define CART_H

#include <glm-1.0.1/glm/glm.hpp>
#include <glm-1.0.1/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <iostream>

#include "Shader.h"
#include "vertex.h"
#include "BezierCurve.h"
#include "ModelLoader.h"

#pragma once
class Cart
{
public:
    Shader* modelShader;
    float size = 8.0f;
    Cart(const std::string& path);
    ~Cart();
    
    // Draw cart to the world
    void Draw();

    // Move cart on the track
    void Move(float distanceAlongCurve, BezierCurve& currentCurve);

    // Setters and Getters
    glm::mat4 getModel();
    void setModel(glm::mat4& model);
    void setView(glm::mat4& view);
    void setProjection(glm::mat4& projection);
    glm::vec3 getPosition();
    glm::vec3 getDirection();
private:
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    glm::mat4 modelMatrix;
    glm::vec3 position;
    glm::vec3 front = glm::vec3(1.0f, 0.0f, 0.0f), right = glm::vec3(0.0f, 0.0f, 0.1f), up = glm::vec3(0.0f, 1.0f, 0.0f);
    bool useCustomModel;
    ModelLoader* modelLoader;

    void setupCart();
};

#endif