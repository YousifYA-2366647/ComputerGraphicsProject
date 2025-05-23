#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <vector>
#include <glm-1.0.1/glm/glm.hpp>
#include "Shader.h"
#include "vertex.h"
#include "arcLengthEntry.h"

#pragma once
class BezierCurve
{
public:
	std::vector<Vertex> controlPoints;
	std::vector<ArcLengthEntry> lookupTable;
	Shader* curveShader;

	BezierCurve(std::vector<Vertex> controlPoints);
	~BezierCurve();
	void Draw();

	float getTotalLength() const;
	glm::vec3 calculateBezierDerivative(float sample);
	glm::mat4 getModel();
	void setModel(glm::mat4& newModel);
	void setView(glm::mat4& newView);
	void setProjection(glm::mat4& newProjection);
private:
	unsigned int VAO, VBO;
	glm::mat4 curveModel = glm::mat4(1.0f);
	void setupCurve();
	glm::vec3 calculateBezierPoint(float sample);
	glm::vec3 calculateBezierColor(float sample);
	int combination(int n, int r);
	int factorial(int n);
};

#endif