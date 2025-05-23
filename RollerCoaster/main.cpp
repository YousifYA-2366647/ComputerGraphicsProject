#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Application.h"

/// functions
void initializeGLFW();

int main() {
	initializeGLFW();

	Application* app = new Application();

	delete app;

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