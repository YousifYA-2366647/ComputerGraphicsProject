#ifndef TRACKPIECE_H
#define TRACKPIECE_H

#include <vector>
#include "Shader.h"
#include "vertex.h"
#include "ModelLoader.h"

#pragma once
class TrackPiece
{
public:
	Shader* trackShader;
	bool loadedCorrectly;
	float size = 1.0f;
	TrackPiece(const std::string& path);
	~TrackPiece();
	void Draw();
	void setModel(glm::mat4& newModel);
	void setView(glm::mat4& newView);
	void setProjection(glm::mat4& newProjection);
private:
	unsigned int VAO, VBO, EBO;
	std::string directory;
	ModelLoader* trackLoader;
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	glm::mat4 trackModel;
	void setupTrackPiece();
};

#endif