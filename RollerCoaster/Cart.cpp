#include "Cart.h"

Cart::Cart(const std::string& path) {
    useCustomModel = true;
    modelLoader = new ModelLoader(path);

    if (!modelLoader->loadModel(path)) {
        std::cerr << "Failed to load model from path: " << path << ". Using default cart model instead." << std::endl;
        useCustomModel = false;

        // Set up the default cart vertices and indices
        m_vertices = {
            // Position                             // Color
            {glm::vec3(0.3f, 0.2f, 0.2f),          glm::vec3(0.0f, 0.5f, 0.5f)},    // Left Bottom Front
            {glm::vec3(-0.3f, 0.2f, 0.2f),         glm::vec3(0.0f, 0.5f, 0.5f)},    // Left Bottom Back
            {glm::vec3(0.3f, -0.2f, 0.2f),         glm::vec3(0.0f, 0.5f, 0.5f)},    // Left Top Front
            {glm::vec3(-0.3f, -0.2f, 0.2f),        glm::vec3(0.0f, 0.5f, 0.5f)},    // Left Top Back
            {glm::vec3(0.3f, 0.2f, -0.2f),         glm::vec3(0.0f, 0.5f, 0.5f)},    // Right Bottom Front
            {glm::vec3(-0.3f, 0.2f, -0.2f),        glm::vec3(0.0f, 0.5f, 0.5f)},    // Right Bottom Back
            {glm::vec3(0.3f, -0.2f, -0.2f),        glm::vec3(1.0f, 1.0f, 1.0f)},    // Right Top Front
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

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.05f, 0.05f) * size);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    setupCart();
}

Cart::~Cart() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete modelShader;
    delete modelLoader;
}

void Cart::Draw() {
    modelShader->use();
    modelShader->setMat4("model", modelMatrix);
    glBindVertexArray(VAO);

    if (useCustomModel) {

        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }
    else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }
}

void Cart::Move(float distanceAlongCurve, BezierCurve& currentCurve) {
    // Move the cart along the curve

    float curveLength = currentCurve.lookupTable.back().arcLength;
    glm::vec3 position = currentCurve.lookupTable.back().arcVertex.Position;
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);

    // Find the correct position
    for (size_t i = 1; i < currentCurve.lookupTable.size(); ++i) {
        if (currentCurve.lookupTable[i].arcLength >= distanceAlongCurve) {
            const auto& a = currentCurve.lookupTable[i - 1];
            const auto& b = currentCurve.lookupTable[i];

            float range = b.arcLength - a.arcLength;
            float f = (distanceAlongCurve - a.arcLength) / range;

            if (range == 0.0f) {
                f = 0.0f;
            }

            glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            // Interpolate the position for smooth movement
            glm::vec3 interpPos = glm::mix(a.arcVertex.Position, b.arcVertex.Position, f);

            glm::vec4 worldPos = currentCurve.getModel() * glm::vec4(interpPos, 1.0f);
            position = glm::mix(this->position, glm::vec3(worldPos), 0.5f);

            // Calculate the direction in which the cart has to look
            float t = glm::mix(a.t, b.t, f);
            direction = currentCurve.calculateBezierDerivative(t);

            direction = glm::normalize(glm::mat3(currentCurve.getModel()) * direction);
            break;
        }
    }

    // Set the results into the cart
    this->position = position + glm::vec3(0.0f, 0.08f, 0.0f) * size;
    this->front = glm::normalize(direction) * -1.0f;

    glm::vec3 worldUp = glm::vec3(0.0f, -1.0f, 0.0f);
    this->right = glm::normalize(glm::cross(worldUp, this->front));

    if (glm::length(this->right) < 0.001f) {
        this->right = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    this->up = glm::normalize(glm::cross(this->right, this->front));

    glm::mat4 view = glm::lookAt(
        this->position,
        this->position + this->front,
        this->up
    );

    glm::mat4 transform = glm::inverse(view);
    modelMatrix = glm::scale(transform, glm::vec3(0.1f, 0.1f, 0.2f) * size);
}

void Cart::setupCart() {
    modelShader = new Shader("cartVertexShader.vert", "cartFragmentShader.frag");

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

void Cart::setModel(glm::mat4& newModel) {
    modelShader->use();
    modelShader->setMat4("model", newModel);

    this->modelMatrix = newModel;
}

void Cart::setView(glm::mat4& newView) {
    modelShader->use();
    modelShader->setMat4("view", newView);
}

void Cart::setProjection(glm::mat4& newProjection) {
    modelShader->use();
    modelShader->setMat4("projection", newProjection);
}

glm::vec3 Cart::getPosition() {
    return position;
}

glm::vec3 Cart::getDirection() {
    return front * -1.0f;
}