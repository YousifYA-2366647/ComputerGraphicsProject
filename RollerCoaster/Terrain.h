#ifndef TERRAIN_H
#define TERRAIN_H

#include <glm-1.0.1/glm/glm.hpp>

#include <vector>

#include "Shader.h"
#include "vertex.h"

#pragma once
class Terrain
{
public:
	Terrain();
	~Terrain();
	void Draw(Shader& shader);
private:
	unsigned int VAO, VBO, EBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	void setupTerrain();
};

#endif