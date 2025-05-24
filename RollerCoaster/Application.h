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
#include "Camera.h"
#include "UIPanel.h"
#include "Light.h"
#pragma once
class Application
{
public:
	Application();
	~Application();

	void setInputMode(int mode, int value);
	void setCursorPosCallback(GLFWcursorposfun callback);
	void setScrollCallback(GLFWscrollfun callback);
	void setMouseButtonCallback(GLFWmousebuttonfun callback);
	void setFramebufferSizeCallback(GLFWframebuffersizefun callback);

	void runWindow();
	// Global/free camera startposities
	Camera globalCamera = Camera(glm::vec3(0.0f, 2.0f, 10.0f), glm::vec3(0, 1, 0), -90.0f, 0.0f);
	Camera firstPersonCamera = Camera(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0, 1, 0), -90.0f, 0.0f);

	

	/// window settings
	unsigned int WIDTH = 1800;
	unsigned int HEIGHT = 1000;

	bool showPanel = false;
	bool panelDragActive = false;
	bool firstPersonView = false;
    bool wasPriorFirstPerson = false;
    bool firstPersonLookingAround = false;  
    glm::vec3 savedFirstPersonFront;  

	UIPanel* panel;
	LightManager lightManager;
	
    
    
private:
	void processInput(float deltaTime);
	GLFWwindow* window;
  
	// Key mapping
	unsigned int quitKey = GLFW_KEY_ESCAPE;
	unsigned int moveForward = GLFW_KEY_W;
	unsigned int moveBackward = GLFW_KEY_S;
	unsigned int moveRight = GLFW_KEY_D;
	unsigned int moveLeft = GLFW_KEY_A;
	unsigned int moveUp = GLFW_KEY_E;
	unsigned int moveDown = GLFW_KEY_Q;
	unsigned int switchPOV = GLFW_KEY_F;
	unsigned int togglePanelKey = GLFW_KEY_TAB;

	float lastFrameTime = 0.0f;
	float lastX = WIDTH / 2.0f;
	float lastY = HEIGHT / 2.0f;
	bool firstMouse = true;
	float speed = 2.0f;

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