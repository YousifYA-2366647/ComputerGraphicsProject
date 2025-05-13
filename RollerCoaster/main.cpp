#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm-1.0.1/glm/glm.hpp>
#include <glm-1.0.1/glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"

// window settings
unsigned int WIDTH = 1800;
unsigned int HEIGHT = 1000;

// keybinds
unsigned int quitKey = GLFW_KEY_ESCAPE;
unsigned int moveForward = GLFW_KEY_W;
unsigned int moveBackward = GLFW_KEY_S;
unsigned int moveRight = GLFW_KEY_D;
unsigned int moveLeft = GLFW_KEY_A;
unsigned int switchPOV = GLFW_KEY_F;

// model data
float vertices[] = {
	// positions			// colors
	0.5f, -0.5f, 0.0f,		1.0f, 0.0f,	0.0f,
	-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 1.0f
};

// functions
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
	Shader* shader = new Shader("vertexShader.vert", "fragmentShader.frag");

	// Create the vertex array and vertex buffer to store vertices data
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

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

	// Tell the program to use the shader's program
	shader->use();

	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {
		// Process the use input
		processInput(window);

		// Make a cool swirling effect for no reason (flexing)
		float currentTime = glfwGetTime();
		glm::vec3 color1 = glm::vec3(
			sin(currentTime + 0.4f) * 0.5f + 0.5f,
			sin(currentTime + 2.5f) * 0.5f + 0.5f,
			sin(currentTime + 1.3f) * 0.5f + 0.5f
		);
		glm::vec3 color2 = glm::vec3(
			sin(currentTime + 2.0f) * 0.5f + 0.5f,
			sin(currentTime + 0.5f) * 0.5f + 0.5f,
			sin(currentTime + 1.1f) * 0.5f + 0.5f
		);
		glm::vec3 color3 = glm::vec3(
			sin(currentTime + 2.0f) * 0.5f + 0.5f,
			sin(currentTime + 2.5f) * 0.5f + 0.5f,
			sin(currentTime + 3.0f) * 0.5f + 0.5f
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