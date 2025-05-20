#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <vector>
#include <glm-1.0.1/glm/glm.hpp>
#include "Shader.h"

#pragma once
class BezierCurve
{
public:
	std::vector<glm::vec3> controlPoints;
	std::vector<glm::vec3> vertices;

	BezierCurve(std::vector<glm::vec3> controlPoints);
	~BezierCurve();
	void Draw(Shader& shader);
private:
	unsigned int VAO, VBO;
	void setupCurve();
	glm::vec3 calculateBezierCurve(float sample);
};

#endif