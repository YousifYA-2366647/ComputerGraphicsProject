#ifndef LIGHTHOLDER_H
#define LIGHTHOLDER_H

#include "Shader.h"
#include "vertex.h"
#include <vector>

#pragma once
class LightHolder
{
public:
	LightHolder(const glm::vec3& lightPosition, bool shouldFlip);
	~LightHolder();
	void Draw();

	// Setters and Getters
	void setView(glm::mat4& newView);
	void setProjection(glm::mat4& newProjection);
	Shader* getShader();
private:
	unsigned int VAO, VBO, EBO;
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	glm::vec3 m_lightPosition;
	glm::mat4 lightHolderModel;
	Shader* lightHolderShader;
	void setupLightHolder();

	// Flip the object around the y-axis for if the building it's attached to is on another side
	void flip();
};

#endif