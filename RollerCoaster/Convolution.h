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
	void Bloom(float threshold, float blurIntensity, int blurPasses = 10);
	void bindBuffer();
	void bindHDR();
	void resize(unsigned int newWidth, unsigned int newHeight);
private:
	Shader* blurShader;
	Shader* edgeShader;
	Shader* invertShader;
	Shader* grayShader;
	Shader* sharpenShader;
	Shader* bloomExtractShader;
	Shader* bloomBlurShader;
	Shader* bloomCombineShader;

	unsigned int VAO, VBO, FBO, RBO, textureColorBuffer, hdrFBO, hdrRBO;
	unsigned int colorBuffers[2];
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];

	std::vector<float> quadVertices;

	unsigned int screenWidth, screenHeight;

	void setupConvolution();
	void setupPingpongBuffers();
	void draw(Shader* shader, float intensity);
};

#endif