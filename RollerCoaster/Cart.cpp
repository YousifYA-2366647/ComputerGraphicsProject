#include "Cart.h"

Cart::Cart() {
    useCustomModel = false;

    m_vertices = {
        // Position                             // Color
        {glm::vec3(0.3f, 0.2f, 0.2f),          glm::vec3(0.0f, 0.5f, 0.0f)},    // Left Bottom Front
        {glm::vec3(-0.3f, 0.2f, 0.2f),         glm::vec3(0.0f, 0.5f, 0.5f)},    // Left Bottom Back
        {glm::vec3(0.3f, -0.2f, 0.2f),         glm::vec3(0.0f, 0.5f, 0.0f)},    // Left Top Front
        {glm::vec3(-0.3f, -0.2f, 0.2f),        glm::vec3(0.0f, 0.5f, 0.5f)},    // Left Top Back
        {glm::vec3(0.3f, 0.2f, -0.2f),         glm::vec3(0.0f, 0.5f, 0.0f)},    // Right Bottom Front
        {glm::vec3(-0.3f, 0.2f, -0.2f),        glm::vec3(0.0f, 0.5f, 0.5f)},    // Right Bottom Back
        {glm::vec3(0.3f, -0.2f, -0.2f),        glm::vec3(0.0f, 0.5f, 0.0f)},    // Right Top Front
        {glm::vec3(-0.3f, -0.2f, -0.2f),       glm::vec3(0.0f, 0.5f, 0.5f)},    // Right Top Back
    };

    m_indices = {
        0, 1, 3,
        0, 2, 3,
        0, 2, 4,
        2, 4, 6,
        4, 5, 6,
        5, 6, 7,
        1, 5, 7,
        1, 3, 7,
        2, 3, 6,
        3, 6, 7,
        0, 1, 4,
        1, 4, 5
    };

    setupCart();
}

Cart::Cart(const std::string& path) {
    useCustomModel = true;

    if (!loadModel(path)) {
        std::cerr << "Failed to load model from path: " << path << ". Using default cart model instead." << std::endl;
        useCustomModel = false;

        // Set up the default cart vertices and indices (same as in the default constructor)
        m_vertices = {
            // Position                             // Color
            {glm::vec3(0.3f, 0.2f, 0.2f),          glm::vec3(0.5f, 0.0f, 0.0f)},    // Left Bottom Front
            {glm::vec3(-0.3f, 0.2f, 0.2f),         glm::vec3(0.0f, 0.5f, 0.5f)},    // Left Bottom Back
            {glm::vec3(0.3f, -0.2f, 0.2f),         glm::vec3(0.5f, 0.0f, 0.0f)},    // Left Top Front
            {glm::vec3(-0.3f, -0.2f, 0.2f),        glm::vec3(0.0f, 0.5f, 0.5f)},    // Left Top Back
            {glm::vec3(0.3f, 0.2f, -0.2f),         glm::vec3(0.5f, 0.0f, 0.0f)},    // Right Bottom Front
            {glm::vec3(-0.3f, 0.2f, -0.2f),        glm::vec3(0.0f, 0.5f, 0.5f)},    // Right Bottom Back
            {glm::vec3(0.3f, -0.2f, -0.2f),        glm::vec3(0.5f, 0.0f, 0.0f)},    // Right Top Front
            {glm::vec3(-0.3f, -0.2f, -0.2f),       glm::vec3(0.0f, 0.5f, 0.5f)},    // Right Top Back
        };

        m_indices = {
            0, 1, 3,
            0, 2, 3,
            0, 2, 4,
            2, 4, 6,
            4, 5, 6,
            5, 6, 7,
            1, 5, 7,
            1, 3, 7,
            2, 3, 6,
            3, 6, 7,
            0, 1, 4,
            1, 4, 5
        };
    }

    setupCart();
}

Cart::~Cart() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Cart::Draw(Shader& shader) {
    shader.use();
    glBindVertexArray(VAO);

    if (useCustomModel) {
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }
    else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }
}

void Cart::Move(glm::vec3& position, glm::vec3& direction) {
    this->position = position + glm::vec3(0.0f, 0.2f, 0.0f);
    this->front = glm::normalize(direction);

    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    this->right = glm::normalize(glm::cross(worldUp, this->front));

    if (glm::length(this->right) < 0.001f) {
        this->right = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    this->up = glm::normalize(glm::cross(this->front, this->right));

    glm::mat4 view = glm::lookAt(
        this->position,
        this->position + this->front,
        this->up
    );

    modelMatrix = glm::inverse(view);
}

void Cart::setupCart() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // Assign the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    // Assign the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

glm::mat4 Cart::getModel() {
    return modelMatrix;
}

bool Cart::loadModel(const std::string& path) {
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

    // Process scene graph and extract mesh data
    processNode(scene->mRootNode, scene);

    return true;
}

void Cart::processNode(aiNode* node, const aiScene* scene) {
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

void Cart::processMesh(aiMesh* mesh, const aiScene* scene) {
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