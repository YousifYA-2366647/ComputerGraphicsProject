#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm-1.0.1/glm/glm.hpp>
#include <glm-1.0.1/glm/gtc/type_ptr.hpp>
#include <glm-1.0.1/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include "Shader.h"

/// window settings
unsigned int WIDTH = 1800;
unsigned int HEIGHT = 1000;

// keybinds
unsigned int quitKey = GLFW_KEY_ESCAPE;
unsigned int moveForward = GLFW_KEY_W;
unsigned int moveBackward = GLFW_KEY_S;
unsigned int moveRight = GLFW_KEY_D;
unsigned int moveLeft = GLFW_KEY_A;
unsigned int switchPOV = GLFW_KEY_F;

/// model data
float vertices[] = {
	// positions			// colors
	0.5f, -0.5f, 0.0f,		1.0f, 0.0f,	0.0f,
	-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 1.0f
};

// bezier curve data
glm::vec3 upperCurve[] = {
	glm::vec3(1.0f, -0.01f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(-1.0f, 1.0f, 0.0f),
	glm::vec3(-1.0f, -0.01f, 0.0f),
};

glm::vec3 lowerCurve[] = {
	glm::vec3(-1.0f, 0.01f, 0.0f),
	glm::vec3(-1.0f, -1.0f, 0.0f),
	glm::vec3(1.0f, -1.0f, 0.0f),
	glm::vec3(1.0f, 0.01f, 0.0f),
};

/// functions
void initializeGLFW();
GLFWwindow* createGLFWwindow(int width, int height);
void runWindow(GLFWwindow* window);
void resizedWindow(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
glm::vec3 calculateBezierCurve(glm::vec3 controlPoints[], float sample);

int main() {
	initializeGLFW();

	GLFWwindow* window = createGLFWwindow(WIDTH, HEIGHT);

	runWindow(window);

	glfwTerminate();
	return 0;
}

// Initialize the GLFW environment
void initializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

// Create a window with the given width and height
GLFWwindow* createGLFWwindow(int width, int height) {
	GLFWwindow* window = glfwCreateWindow(width, height, "Roller Coaster", NULL, NULL);

	if (window == NULL) {
		// Exit if the window couldn't be created
		std::cout << "Couldn't create a GLFW window... Terminating." << std::endl;
		glfwTerminate();
		std::exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, resizedWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		// Exit if GLAD couldn't be initialized
		std::cout << "Couldn't initialize GLAD... Terminating." << std::endl;
		glfwTerminate();
		std::exit(-2);
	}

	return window;
}

// Runs a loop that keeps rendering the given window
void runWindow(GLFWwindow* window) {
	// Create a 'Shader' object to store the vertex and fragment shader in
	Shader* shader = new Shader("vertexShader.vert", "fragmentShader.frag");

	// Create the vertex array and vertex buffer to store vertices data
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	/// model vertex binding
	// Bind the array and buffer
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Assign the position attribute (you can find the values at the top in the variable 'vertices')
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Assign the color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	/// calculate bezier curve
	std::vector<glm::vec3> upperCurvePoints = {};
	int steps = 1000;

	for (int i = 0; i < steps; i++) {
		float t = static_cast<float>(i) / static_cast<float>(steps);
		upperCurvePoints.push_back(calculateBezierCurve(upperCurve, t));
	}

	std::vector<glm::vec3> lowerCurvePoints = {};

	for (int i = 0; i < steps; i++) {
		float t = static_cast<float>(i) / static_cast<float>(steps);
		lowerCurvePoints.push_back(calculateBezierCurve(lowerCurve, t));
	}

	/// upper curve vertex binding
	unsigned int upperBCVAO, upperBCVBO;
	glGenVertexArrays(1, &upperBCVAO);
	glGenBuffers(1, &upperBCVBO);

	glBindVertexArray(upperBCVAO);

	// upper curve
	glBindBuffer(GL_ARRAY_BUFFER, upperBCVBO);
	glBufferData(GL_ARRAY_BUFFER, upperCurvePoints.size() * sizeof(glm::vec3), upperCurvePoints.data(), GL_STATIC_DRAW);

	// Assign the position attribute (you can find the values at the top in the variable 'vertices')
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	/// lower curve vertex binding
	unsigned int lowerBCVAO, lowerBCVBO;
	glGenVertexArrays(1, &lowerBCVAO);
	glGenBuffers(1, &lowerBCVBO);

	glBindVertexArray(lowerBCVAO);

	// lower curve
	glBindBuffer(GL_ARRAY_BUFFER, lowerBCVBO);
	glBufferData(GL_ARRAY_BUFFER, lowerCurvePoints.size() * sizeof(glm::vec3), lowerCurvePoints.data(), GL_STATIC_DRAW);

	// Assign the position attribute (you can find the values at the top in the variable 'vertices')
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	// Tell the program to use the shader's program
	shader->use();

	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {
		// Process the use input
		processInput(window);

		// Make a cool swirling effect for no reason (flexing)
		float currentTime = glfwGetTime();
		glm::vec3 color1 = glm::vec3(
			sin(currentTime + 10.0f) * 0.5f + 1.0f,
			sin(currentTime + 2.5f) * 0.5f + 1.0f,
			sin(currentTime + 1.3f) * 0.5f + 1.0f
		);
		glm::vec3 color2 = glm::vec3(
			sin(currentTime + 2.0f) * 0.5f + 1.0f,
			sin(currentTime + 3.5f) * 0.5f + 1.0f,
			sin(currentTime + 1.1f) * 0.5f + 1.0f
		);
		glm::vec3 color3 = glm::vec3(
			sin(currentTime + 2.0f) * 0.5f + 1.0f,
			sin(currentTime + 2.5f) * 0.5f + 1.0f,
			sin(currentTime + 3.0f) * 0.5f + 1.0f
		);

		// Update the calculated color values in the fragment shader
		shader->use();
		shader->setVec3("color1", color1);
		shader->setVec3("color2", color2);
		shader->setVec3("color3", color3);

		// Give the window a background color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the vertices and connect them to make triangles
		shader->use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		shader->use();
		glBindVertexArray(upperBCVAO);
		glDrawArrays(GL_LINE_STRIP, 0, upperCurvePoints.size());

		shader->use();
		glBindVertexArray(lowerBCVAO);
		glDrawArrays(GL_LINE_STRIP, 0, lowerCurvePoints.size());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Delete the arrays upon completion
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

// Resize the viewport along with the window whenever the user resizes the window
void resizedWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// Process user input
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, quitKey) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

glm::vec3 calculateBezierCurve(glm::vec3 controlPoints[], float sample) {
	float oneMinusSample = 1 - sample;
	float oneMinusSampleSquared = oneMinusSample * oneMinusSample;
	float oneMinusSampleCubed = oneMinusSampleSquared * oneMinusSample;

	float sampleSquared = sample * sample;
	float sampleCubed = sample * sampleSquared;

	return oneMinusSampleCubed * controlPoints[0] +
		3 * oneMinusSampleSquared * sample * controlPoints[1] +
		3 * oneMinusSample * sampleSquared * controlPoints[2] +
		sampleCubed * controlPoints[3];
}