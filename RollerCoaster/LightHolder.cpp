#include "LightHolder.h"

LightHolder::LightHolder(const glm::vec3& lightPosition, bool shouldFlip): m_lightPosition(lightPosition) {
	m_vertices = {
		// Rectangle 1
		// Position								// Color
		{glm::vec3(0.0f, 0.0f, 0.0f),			glm::vec3(0.3f, 0.3f, 0.3f)},	// Top near right
		{glm::vec3(-1.0f, 0.0f, 0.0f),			glm::vec3(0.3f, 0.3f, 0.3f)},	// Top near left
		{glm::vec3(0.0f, -0.2f, 0.0f),			glm::vec3(0.3f, 0.3f, 0.3f)},	// Bottom near right
		{glm::vec3(-1.0f, -0.2f, 0.0f),			glm::vec3(0.3f, 0.3f, 0.3f)},	// Bottom near left
		{glm::vec3(0.0f, 0.0f, -0.2f),			glm::vec3(0.3f, 0.3f, 0.3f)},	// Top far right
		{glm::vec3(-1.0f, 0.0f, -0.2f),			glm::vec3(0.3f, 0.3f, 0.3f)},	// Top far left
		{glm::vec3(0.0f, -0.2f, -0.2f),			glm::vec3(0.3f, 0.3f, 0.3f)},	// Bottom far right
		{glm::vec3(-1.0f, -0.2f, -0.2f),		glm::vec3(0.3f, 0.3f, 0.3f)},	// Bottom far left

		// Rectangle 2
		// Position								// Color
		{glm::vec3(-0.8f, -0.2f, 0.0f),			glm::vec3(0.3f, 0.3f, 0.3f)},	// Top near right
		{glm::vec3(-0.8f, -0.6f, 0.0f),			glm::vec3(0.3f, 0.3f, 0.3f)},	// Bottom near right
		{glm::vec3(-1.0f, -0.6f, 0.0f),			glm::vec3(0.3f, 0.3f, 0.3f)},	// Bottom near left
		{glm::vec3(-0.8f, -0.2f, -0.2f),		glm::vec3(0.3f, 0.3f, 0.3f)},	// Top far right
		{glm::vec3(-0.8f, -0.6f, -0.2f),		glm::vec3(0.3f, 0.3f, 0.3f)},	// Bottom far right
		{glm::vec3(-1.0f, -0.6f, -0.2f),		glm::vec3(0.3f, 0.3f, 0.3f)},	// Bottom far left
	};

	m_indices = {
		// Rectangle 1
		0, 2, 3,
		0, 1, 3,
		0, 2, 4,
		2, 4, 6,
		1, 3, 7,
		1, 5, 7,
		2, 3, 6,
		3, 6, 7,
		0, 1, 4,
		1, 4, 5,
		4, 5, 6,
		5, 6, 7,

		// Rectangle 2
		8, 9, 10,
		8, 3, 10,
		3, 7, 13,
		3, 10, 13,
		8, 9, 12,
		8, 11, 12,
		11, 12, 13,
		11, 7, 13,
	};

	setupLightHolder();

	if (shouldFlip) {
		flip();
	}
}

LightHolder::~LightHolder() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	delete lightHolderShader;
}

void LightHolder::Draw() {
	lightHolderShader->use();
	lightHolderShader->setMat4("model", lightHolderModel);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

void LightHolder::setupLightHolder() {
	lightHolderShader = new Shader("cartVertexShader.vert", "cartFragmentShader.frag");

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	// Bind position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);

	// Bind color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	lightHolderModel = glm::mat4(1.0f);
	lightHolderModel = glm::translate(lightHolderModel, m_lightPosition + glm::vec3(0.1f, 0.7f, -0.9f));
	lightHolderModel = glm::rotate(lightHolderModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightHolderShader->use();
	lightHolderShader->setMat4("model", lightHolderModel);
}

void LightHolder::setView(glm::mat4& newView) {
	lightHolderShader->use();
	lightHolderShader->setMat4("view", newView);
}

void LightHolder::setProjection(glm::mat4& newProjection) {
	lightHolderShader->use();
	lightHolderShader->setMat4("projection", newProjection);
}

Shader* LightHolder::getShader() {
	return lightHolderShader;
}

void LightHolder::flip() {
	// Flip the holder and move it to be once again right above the light
	lightHolderModel = glm::rotate(lightHolderModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightHolderModel = glm::translate(lightHolderModel, glm::vec3(1.8f, 0.0f, 0.2f));
	lightHolderShader->use();
	lightHolderShader->setMat4("model", lightHolderModel);
}