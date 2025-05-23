#include "Terrain.h"

Terrain::Terrain() {
	vertices = {
		{glm::vec3(50.0f, -20.0f, 50.0f),			glm::vec3(0.0f, 0.3f, 0.0f)},
		{glm::vec3(-50.0f, -20.0f, 50.0f),			glm::vec3(0.0f, 0.3f, 0.0f)},
		{glm::vec3(50.0f, -20.0f, -50.0f),			glm::vec3(0.0f, 0.3f, 0.0f)},
		{glm::vec3(-50.0f, -20.0f, -50.0f),			glm::vec3(0.0f, 0.3f, 0.0f)},
	};

	indices = {
		0, 1, 2,
		1, 2, 3
	};

	setupTerrain();
}

Terrain::~Terrain() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Terrain::Draw() {
	terrainShader->use();
	terrainShader->setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Terrain::setupTerrain() {
	terrainShader = new Shader("terrainVertexShader.vert", "terrainFragmentShader.frag");

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Assign the position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);

	// Assign the color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	model = glm::mat4(1.0f);
	terrainShader->use();
	terrainShader->setMat4("model", model);
}

void Terrain::setModel(glm::mat4& newModel) {
	this->terrainShader->use();
	terrainShader->setMat4("model", newModel);
	this->model = newModel;
}

void Terrain::setView(glm::mat4& newView) {
	this->terrainShader->use();
	terrainShader->setMat4("view", newView);
}

void Terrain::setProjection(glm::mat4& newProjection) {
	this->terrainShader->use();
	terrainShader->setMat4("projection", newProjection);
}