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

float lastFrameTime = 0.0f;

// bezier curve data
std::vector<Vertex> upperCurvePoints = {
	// Position							         // Color
	{glm::vec3(0.0f, 0.0f, -0.2f),		       glm::vec3(1.0f, 1.0f, 1.0f)},
	{glm::vec3(1.5f, 2.2f, -0.8f),		       glm::vec3(1.0f, 0.0f, 1.0f)},
	{glm::vec3(-1.2f, 3.0f, 1.2f),		       glm::vec3(1.0f, 1.0f, 0.0f)},
	{glm::vec3(1.0f, 1.8f, 0.2f),		       glm::vec3(0.0f, 1.0f, 1.0f)},
	{glm::vec3(0.0f, 0.0f, 0.8f),		       glm::vec3(1.0f, 0.0f, 1.0f)}
};

std::vector<Vertex> lowerCurvePoints = {
	{glm::vec3(0.0f, 0.0f, -0.2f),		       glm::vec3(1.0f, 1.0f, 1.0f)},
	{glm::vec3(-1.2f, -1.8f, -1.2f),		   glm::vec3(0.0f, 1.0f, 1.0f)},
	{glm::vec3(1.5f, -2.8f, 1.3f),		       glm::vec3(1.0f, 1.0f, 0.0f)},
	{glm::vec3(-1.0f, -1.5f, 1.0f),		       glm::vec3(1.0f, 0.0f, 1.0f)},
	{glm::vec3(0.0f, 0.0f, 0.8f),		       glm::vec3(1.0f, 0.0f, 1.0f)}
};

/// functions
void initializeGLFW();
GLFWwindow* createGLFWwindow(int width, int height);
void runWindow(GLFWwindow* window);
void resizedWindow(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

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
	Shader* curveShader = new Shader("vertexShader.vert", "fragmentShader.frag");
	Shader* modelShader = new Shader("cartVertexShader.vert", "cartFragmentShader.frag");

	/// calculate bezier curve
	BezierCurve* upperCurve = new BezierCurve(upperCurvePoints);

	BezierCurve* lowerCurve = new BezierCurve(lowerCurvePoints);

	Cart* cartObject = new Cart(std::string("C:/Users/yousi/ComputerGraphicsProject/RollerCoaster/model/rollerCoaster.obj"));

	// Tell the program to use the shader's program
	curveShader->use();

	glm::mat4 curveModel = glm::mat4(1.0f);
	curveModel = glm::rotate(curveModel, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	curveModel = glm::rotate(curveModel, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 cartModel = glm::mat4(1.0f);
	cartModel = glm::rotate(cartModel, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);

	curveShader->setMat4("model", curveModel);
	curveShader->setMat4("view", view);
	curveShader->setMat4("projection", projection);

	modelShader->use();

	modelShader->setMat4("model", cartModel);
	modelShader->setMat4("view", view);
	modelShader->setMat4("projection", projection);

	glEnable(GL_DEPTH_TEST);

	float currentTime = glfwGetTime();
	lastFrameTime = currentTime;

	bool onUpper = true;

	float speed = 2.0f; // units per second
	float distanceAlongCurve = 0.0f;
	float upperCurveLength = upperCurve->lookupTable.back().arcLength;
	float lowerCurveLength = lowerCurve->lookupTable.back().arcLength;

	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {
		currentTime = glfwGetTime();
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		// Process the use input
		processInput(window);

		// Make a cool rotating effect for no reason (flexing)
		float currentTime = glfwGetTime();

		//curveModel = glm::rotate(curveModel, glm::radians(30.0f) * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));

		//// Update the calculated transform values in the fragment shader
		//curveShader->use();
		//curveShader->setMat4("model", curveModel);

		// Give the window a background color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		upperCurve->Draw(*curveShader);
		lowerCurve->Draw(*curveShader);

		// Update the distance along the current curve
		distanceAlongCurve += speed * deltaTime;

		if (onUpper && distanceAlongCurve >= upperCurveLength) {
			onUpper = false;
			distanceAlongCurve = 0.0f; // reset for lower curve
		}
		else if (!onUpper && distanceAlongCurve >= lowerCurveLength) {
			onUpper = true;
			distanceAlongCurve = 0.0f; // reset for upper curve
		}

		// Choose the right lookup table
		std::vector<ArcLengthEntry>& table = onUpper ? upperCurve->lookupTable : lowerCurve->lookupTable;
		BezierCurve* currentCurve = onUpper ? upperCurve : lowerCurve;

		float effectiveDistance = distanceAlongCurve;

		if (!onUpper) {
			// Traverse lower curve in reverse
			float lowerLength = lowerCurve->lookupTable.back().arcLength;
			effectiveDistance = lowerLength - distanceAlongCurve;
		}

		// Find the position at this effective distance using linear search
		glm::vec3 position = table.back().arcVertex.Position; // fallback
		glm::vec3 direction;
		for (size_t i = 1; i < table.size(); ++i) {
			if (table[i].arcLength >= effectiveDistance) {
				const auto& a = table[i - 1];
				const auto& b = table[i];

				float range = b.arcLength - a.arcLength;
				float f = (effectiveDistance - a.arcLength) / range;

				if (range == 0.0f) {
					f = 0.0f;
				}

				// If moving in reverse, flip interpolation direction
				glm::vec3 interpPos = onUpper ?
					glm::mix(a.arcVertex.Position, b.arcVertex.Position, f) :
					glm::mix(b.arcVertex.Position, a.arcVertex.Position, f);

				glm::vec4 worldPos = curveModel * glm::vec4(interpPos, 1.0f);
				position = glm::vec3(worldPos);

				float t = glm::mix(a.t, b.t, f);
				direction = currentCurve->calculateBezierDerivative(t);

				break;
			}
		}

		direction = glm::normalize(direction);

		// Move and draw the cart
		cartObject->Move(position, direction);
		modelShader->use();
		modelShader->setMat4("model", cartObject->getModel());
		cartObject->Draw(*modelShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete curveShader;
	delete modelShader;
	delete upperCurve;
	delete lowerCurve;
	delete cartObject;
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