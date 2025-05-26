#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <iostream>

#include "Shader.h"
#include "stb_image.h"

#pragma once
class SkyBox
{
public:
	Shader* skyBoxShader;

	SkyBox();
	~SkyBox();

	// Draws the skybox
	void Draw(glm::mat4 view, glm::mat4 projection);

	// Load cubemap from a list of paths to images
	unsigned int loadCubeMap(std::vector<std::string> faces);
private:
	unsigned int VAO, VBO;
	unsigned int cubeTexture;
	void setupSkyBox();

    std::vector<float> skyBoxVertices;
};

#endif