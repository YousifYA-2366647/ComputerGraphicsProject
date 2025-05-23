#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm-1.0.1/glm/glm.hpp>
#include <glm-1.0.1/glm/gtc/type_ptr.hpp>
#include <glm-1.0.1/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include "Shader.h"
#include "BezierCurve.h"
#include "vertex.h"
#include "Cart.h"

#pragma once
class Application
{
public:
	Application();
private:
	void runWindow();
	void processInput();
	GLFWwindow* window;

	/// window settings
	unsigned int WIDTH = 1800;
	unsigned int HEIGHT = 1000;
	
	// Key mapping
	unsigned int quitKey = GLFW_KEY_ESCAPE;
	unsigned int moveForward = GLFW_KEY_W;
	unsigned int moveBackward = GLFW_KEY_S;
	unsigned int moveRight = GLFW_KEY_D;
	unsigned int moveLeft = GLFW_KEY_A;
	unsigned int switchPOV = GLFW_KEY_F;

	float lastFrameTime = 0.0f;

	// bezier curve data
	std::vector<Vertex> upperCurvePoints = {
		// Position                                   // Color
		{glm::vec3(0.0f, 0.0f, 0.0f),                 glm::vec3(1.0f, 1.0f, 1.0f)},
		{glm::vec3(8.0f, 4.0f, 3.2f),                 glm::vec3(1.0f, 0.5f, 0.0f)},
		{glm::vec3(10.0f, 8.0f, 0.8f),                 glm::vec3(0.5f, 1.0f, 0.0f)},
		{glm::vec3(12.0f, 4.0f, -3.2f),                glm::vec3(0.0f, 0.5f, 1.0f)},
		{glm::vec3(4.0f, -2.0f, 2.0f),                glm::vec3(1.0f, 0.0f, 0.5f)}
	};

	std::vector<Vertex> lowerCurvePoints = {
		// Position                                   // Color
		{glm::vec3(4.0f, -2.0f, 2.0f),				  glm::vec3(1.0f, 0.0f, 0.5f)},
		{glm::vec3(-4.0f, -8.0f, 7.2f),                glm::vec3(1.0f, 0.5f, 0.0f)},
		{glm::vec3(-2.0f, -8.0f, 0.8f),               glm::vec3(0.5f, 1.0f, 0.0f)},
		{glm::vec3(-8.0f, -4.0f, -3.2f),              glm::vec3(0.0f, 0.5f, 1.0f)},
		{glm::vec3(0.0f, 0.0f, 0.0f),				  glm::vec3(1.0f, 1.0f, 1.0f)}
	};
};

#endif