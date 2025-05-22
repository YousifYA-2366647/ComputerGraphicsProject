#ifndef CART_H
#define CART_H

#include <glm-1.0.1/glm/glm.hpp>
#include <vector>
#include <string>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <iostream>

#include "Shader.h"
#include "vertex.h"

#pragma once
class Cart
{
public:
    Cart();
    Cart(const std::string& path);
    ~Cart();
    void Draw(Shader& shader);
    void Move(glm::vec3& position, glm::vec3& direction);
    glm::mat4 getModel();
private:
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
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