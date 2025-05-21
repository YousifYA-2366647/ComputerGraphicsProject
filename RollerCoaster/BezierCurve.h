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

	BezierCurve(std::vector<Vertex> controlPoints);
	~BezierCurve();
	void Draw(Shader& shader);

	float getTotalLength() const;
	glm::vec3 calculateBezierDerivative(float sample);
private:
	unsigned int VAO, VBO;
	void setupCurve();
	glm::vec3 calculateBezierPoint(float sample);
	glm::vec3 calculateBezierColor(float sample);
	int combination(int n, int r);
	int factorial(int n);
};

#endif