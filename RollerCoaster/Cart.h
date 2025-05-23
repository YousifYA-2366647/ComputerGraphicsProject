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

#pragma once
class Cart
{
public:
    Shader* modelShader;
    float size = 2.5f;
    Cart();
    Cart(const std::string& path);
    ~Cart();
    void Draw();
    void Move(float distanceAlongCurve, BezierCurve& currentCurve);
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
    std::string directory;
    glm::mat4 modelMatrix;
    glm::vec3 position;
    glm::vec3 front = glm::vec3(1.0f, 0.0f, 0.0f), right = glm::vec3(0.0f, 0.0f, 0.1f), up = glm::vec3(0.0f, 1.0f, 0.0f);
    bool useCustomModel;

    void setupCart();
    bool loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif