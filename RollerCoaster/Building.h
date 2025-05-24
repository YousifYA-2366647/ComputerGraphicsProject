#ifndef BUILDING_H
#define BUILDING_H

#include "Shader.h"
#include "ModelLoader.h"

#pragma once
class Building
{
public:
	Shader* buildingShader;
	Building(const std::string& path);
	~Building();
	void Draw();
	void scaleModel(glm::vec3 size);
	void translateModel(glm::vec3 position);
	void setView(glm::mat4& view);
	void setProjection(glm::mat4& projection);
private:
	void setupBuilding();
	unsigned int VAO, VBO, EBO;
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	bool useCustomModel = false;
	ModelLoader* modelLoader;
	glm::mat4 buildingModel;
};

#endif