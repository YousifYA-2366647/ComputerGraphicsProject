#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include "Application.h"
#include <iostream>
#include <vector>

#include "Camera.h"
#include "UIPanel.h"
#include "Application.h"

Application* app;

/// functions
void initializeGLFW();
void resizedWindow(GLFWwindow* window, int width, int height);
void handleMouseMovement(GLFWwindow* window, double xpos, double ypos);
void handleScrollInput(GLFWwindow* window, double xoffset, double yoffset);
void handleMouseClick(GLFWwindow* window, int button, int action, int mods);
glm::vec3 getPickingRay(double mouseX, double mouseY, int width, int height, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos);

int main() {
	initializeGLFW();

	app = new Application();

	app->setInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	app->setCursorPosCallback(handleMouseMovement);
	app->setScrollCallback(handleScrollInput);
	app->setMouseButtonCallback(handleMouseClick);
	app->setFramebufferSizeCallback(resizedWindow);

	app->runWindow();

	glfwTerminate();
	delete app;
	return 0;
}

// Initialize the GLFW environment
void initializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

// Resize the viewport along with the window whenever the user resizes the window
void resizedWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void handleMouseMovement(GLFWwindow* window, double xpos, double ypos) {
	static float lastX = app->WIDTH / 2.0f;
    static float lastY = app->HEIGHT / 2.0f;
    static bool firstMouse = true;

    static bool wasRightPressed = false;
    bool rightPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (app->firstPersonView && rightPressed && !wasRightPressed) {
        app->firstPersonLookingAround = true;
        firstMouse = true;  
    }
    
    if (app->firstPersonView && !rightPressed && wasRightPressed) {
        app->firstPersonLookingAround = false;
    }

    if (!app->firstPersonView && rightPressed && !wasRightPressed) {
        firstMouse = true; 
    }
    
    wasRightPressed = rightPressed;

    if (!rightPressed)
        return;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return; 
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (app->firstPersonView && app->firstPersonLookingAround) {
        app->firstPersonCamera.ProcessMouseMovement(xoffset, yoffset, true);
    }
    else if (!app->firstPersonView) {
        app->globalCamera.ProcessMouseMovement(xoffset, yoffset);
    }
	if (app->panelDragActive && app->panel->dragging) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		glm::vec3 camPos = app->firstPersonView ? app->firstPersonCamera.Position : app->globalCamera.Position;

		glm::mat4 view = app->firstPersonView ? app->firstPersonCamera.GetViewMatrix() : app->globalCamera.GetViewMatrix();

		glm::mat4 proj = glm::perspective(
			glm::radians(app->firstPersonView ? 50.0f : app->globalCamera.Zoom),
			float(app->WIDTH) / app->HEIGHT, 0.1f, 100.0f);

		glm::vec3 rayDir = getPickingRay(xpos, ypos, app->WIDTH, app->HEIGHT, view, proj, camPos);

		float dotNormalRay = glm::dot(app->panel->normal, rayDir);
		if (fabs(dotNormalRay) > 1e-6f) {
			float t = glm::dot(app->panel->pos - camPos, app->panel->normal) / dotNormalRay;
			if (t > 0) {
				glm::vec3 hit = camPos + t * rayDir;
				app->panel->updateDrag(hit);
			}
		}
	}

}
void handleScrollInput(GLFWwindow* window, double xoffset, double yoffset) {
	if (!app->firstPersonView) {
		app->globalCamera.ProcessMouseScroll((float)yoffset);
	}
	else {
		app->firstPersonCamera.ProcessMouseScroll((float)yoffset);
	}
}
void handleMouseClick(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS && app->panel->visible) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			glm::vec3 camPos = app->globalCamera.Position;
			glm::mat4 view = app->globalCamera.GetViewMatrix();
			glm::mat4 proj = glm::perspective(glm::radians(app->globalCamera.Zoom), float(app->WIDTH) / app->HEIGHT, 0.1f, 100.0f);
			glm::vec3 rayDir = getPickingRay(xpos, ypos, app->WIDTH, app->HEIGHT, view, proj, camPos);

			int i = app->panel->pickElement(camPos, rayDir);
			if (i == -1) {
				// Ray-plane intersection voor het panel
				float dotNormalRay = glm::dot(app->panel->normal, rayDir);
				if (fabs(dotNormalRay) > 1e-6f) {
					float t = glm::dot(app->panel->pos - camPos, app->panel->normal) / dotNormalRay;
					if (t > 0) { glm::vec3 hit = camPos + t * rayDir; app->panel->startDrag(hit); app->panelDragActive = true;
					}
				}
			}
			else {
				app->panel->processClick(i);
			}
		}
		if (action == GLFW_RELEASE) {
			app->panel->stopDrag();
			app->panelDragActive = false;
		}
	}
}
glm::vec3 getPickingRay(double mouseX, double mouseY, int width, int height, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos) {
	float x = (2.0f * float(mouseX)) / width - 1.0f;
	float y = 1.0f - (2.0f * float(mouseY)) / height;
	glm::vec4 rayClip(x, y, -1.0f, 1.0f);
	glm::mat4 invViewProj = glm::inverse(proj * view);
	glm::vec4 rayWorld = invViewProj * rayClip;
	rayWorld /= rayWorld.w;
	glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld) - camPos);
	return rayDir;
}
