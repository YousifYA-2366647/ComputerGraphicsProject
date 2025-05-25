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
#include "Terrain.h"
#include "SkyBox.h"
#include "Light.h"
#include "Convolution.h"
#include "ChromaKey.h"

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

	// Flag om te switchen tussen views
	bool firstPersonView = false;
	bool showChromaKey = true; 

	/// window settings
	unsigned int WIDTH = 1920;
	unsigned int HEIGHT = 1080;

	bool showPanel = false;
	bool panelDragActive = false;
    bool wasPriorFirstPerson = false;
    bool firstPersonLookingAround = false;  
    glm::vec3 savedFirstPersonFront;  
	glm::mat4 lastRenderView;
    glm::mat4 lastRenderProj;
	int current_width, current_height;

	UIPanel* panel;
	LightManager lightManager;
	ChromaKeyPictureFrame* chromaKeyPictureFrame;
	
	
    
    
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
	unsigned int blurKey = GLFW_KEY_B;
	unsigned int edgeKey = GLFW_KEY_N;
	unsigned int invertKey = GLFW_KEY_I;
	unsigned int grayScaleKey = GLFW_KEY_G;
	unsigned int sharpenKey = GLFW_KEY_U;
	unsigned int toggleChromaKeyKey = GLFW_KEY_C;

	float lastFrameTime = 0.0f;
	float lastX = WIDTH / 2.0f;
	float lastY = HEIGHT / 2.0f;
	bool firstMouse = true;
	float speed = 8.0f;
	float blurIntensity = 0.0f, edgeDetectIntensity = 0.0f, invertIntensity = 0.0f, grayIntensity = 0.0f, sharpenIntensity = 0.0f;
	

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