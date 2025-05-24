#ifndef TERRAIN_H
#define TERRAIN_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm-1.0.1/glm/glm.hpp>
#include <glm-1.0.1/glm/gtx/string_cast.hpp>
#include <glm-1.0.1/glm/gtc/matrix_transform.hpp>

#include <vector>

#include "Shader.h"
#include "vertex.h"
#include "stb_image.h"
#include "Building.h"

#pragma once
class Terrain
{
public:
	Shader* terrainShader;
	Terrain();
	~Terrain();
	void Draw();
	void setModel(glm::mat4& newModel);
	void setView(glm::mat4& newView);
	void setProjection(glm::mat4& newProjection);
private:
	unsigned int VAO, VBO, EBO, textureId;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Building*> buildings;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	void setupTerrain();
};

#endif