#include "Terrain.h"

Terrain::Terrain() {
	vertices = {
		{glm::vec3(50.0f, 0.0f, 50.0f),			glm::vec3(0.0f, 1.0f, 0.0f)},
		{glm::vec3(-50.0f, 0.0f, 50.0f),			glm::vec3(0.0f, 1.0f, 0.0f)},
		{glm::vec3(50.0f, 0.0f, -50.0f),			glm::vec3(0.0f, 1.0f, 0.0f)},
		{glm::vec3(-50.0f, 0.0f, -50.0f),			glm::vec3(0.0f, 1.0f, 0.0f)},
	};

	indices = {

	};

	setupTerrain();
}

Terrain::~Terrain() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Terrain::setupTerrain() {

}