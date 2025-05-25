#include "TrackPiece.h"

TrackPiece::TrackPiece(const std::string& path) {
	directory = path;
	trackLoader = new ModelLoader(directory);
	trackShader = new Shader("cartVertexShader.vert", "cartFragmentShader.frag");
	loadedCorrectly = true;

	if (!trackLoader->loadModel(directory)) {
		loadedCorrectly = false;
	}
	else {
		m_vertices = trackLoader->getVertices();
		m_indices = trackLoader->getIndices();
	}

	setupTrackPiece();
}

TrackPiece::~TrackPiece() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	delete trackLoader;
	delete trackShader;
}

void TrackPiece::Draw() {
	trackShader->use();
	trackShader->setMat4("model", trackModel);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

void TrackPiece::setupTrackPiece() {
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

	trackShader->use();
	trackModel = glm::mat4(1.0f);
	trackModel = glm::scale(trackModel, glm::vec3(1.0f) * size);
	trackShader->setMat4("model", trackModel);
}


void TrackPiece::setModel(glm::mat4& newModel) {
	trackModel = newModel;
	trackShader->use();
	trackShader->setMat4("model", trackModel);
}

void TrackPiece::setView(glm::mat4& newView) {
	trackShader->use();
	trackShader->setMat4("view", newView);
}

void TrackPiece::setProjection(glm::mat4& newProjection) {
	trackShader->use();
	trackShader->setMat4("projection", newProjection);
}