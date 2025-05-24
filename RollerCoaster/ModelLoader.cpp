#include "ModelLoader.h"

ModelLoader::ModelLoader(const std::string& path) {
    directory = path;
}

std::vector<Vertex> ModelLoader::getVertices() {
    return m_vertices;
}

std::vector<unsigned int> ModelLoader::getIndices() {
    return m_indices;
}

bool ModelLoader::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }

    // Clear previous vertices and indices
    m_vertices.clear();
    m_indices.clear();

    directory = path.substr(0, path.find_last_of('/'));

    // Process scene graph and extract mesh data
    processNode(scene->mRootNode, scene);

    return true;
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene) {
    // Process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    // Then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene) {
    // Store the original size of vertices before adding new ones
    unsigned int vertexStartIndex = m_vertices.size();

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Position
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        // Color (default to gray if no vertex colors)
        if (mesh->HasVertexColors(0)) {
            vertex.Color.r = mesh->mColors[0][i].r;
            vertex.Color.g = mesh->mColors[0][i].g;
            vertex.Color.b = mesh->mColors[0][i].b;
        }
        else {
            vertex.Color = glm::vec3(0.7f, 0.7f, 0.7f);
        }

        m_vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            m_indices.push_back(vertexStartIndex + face.mIndices[j]);
        }
    }
}