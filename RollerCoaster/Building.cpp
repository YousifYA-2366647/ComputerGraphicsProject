#include "Building.h"

Building::Building(const std::string& path) {
	modelLoader = new ModelLoader(path);
	useCustomModel = true;

	if (!modelLoader->loadModel(path)) {
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
	}
    else {
        m_vertices = modelLoader->getVertices();
        m_indices = modelLoader->getIndices();
    }

    buildingModel = glm::mat4(1.0f);

    setupBuilding();
}

Building::~Building() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete buildingShader;
    delete modelLoader;
}

void Building::Draw() {
    buildingShader->use();
    buildingShader->setMat4("model", buildingModel);
    glBindVertexArray(VAO);
    if (useCustomModel) {
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }
    else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }
}

void Building::scaleModel(glm::vec3 size) {
    buildingModel = glm::scale(buildingModel, size);
}

void Building::translateModel(glm::vec3 position) {
    buildingModel = glm::translate(buildingModel, position);
}

void Building::setupBuilding() {
    buildingShader = new Shader("cartVertexShader.vert", "cartFragmentShader.frag");

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

void Building::setView(glm::mat4& view) {
    buildingShader->use();
    buildingShader->setMat4("view", view);
}

void Building::setProjection(glm::mat4& projection) {
    buildingShader->use();
    buildingShader->setMat4("projection", projection);
}