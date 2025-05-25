#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <vector>

#include "Shader.h"

#pragma once
class Convolution
{
public:
	Convolution(unsigned int screenWidth, unsigned int screenHeight);
	~Convolution();
	void EdgeDetect(float intensity);
	void Blur(float intensity);
	void Inverse(float intensity);
	void GrayScale(float intensity);
	void Sharpen(float intensity);
	void bindBuffer();
	void resize(unsigned int newWidth, unsigned int newHeight);
private:
	Shader* blurShader;
	Shader* edgeShader;
	Shader* invertShader;
	Shader* grayShader;
	Shader* sharpenShader;
	unsigned int VAO, VBO, FBO, RBO, textureColorBuffer;
	std::vector<float> quadVertices;
	unsigned int screenWidth, screenHeight;
	void setupConvolution();
	void draw(Shader* shader, float intensity);
};

#endif