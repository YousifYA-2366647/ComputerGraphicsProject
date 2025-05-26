#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <vector>
#include <string>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <iostream>

#include "vertex.h"

#pragma once
class ModelLoader
{
public:
    ModelLoader(const std::string& path);

    // Loads model from the given path
    bool loadModel(const std::string& path);

    // Getters for the loaded mesh
    std::vector<Vertex> getVertices();
    std::vector<unsigned int> getIndices();
private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::string directory;
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif