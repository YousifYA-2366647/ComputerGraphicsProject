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
    app->WIDTH = width;
    app->HEIGHT = height;
    app->current_width = width;   
    app->current_height = height; 

    app->framebufferResized = true;

}
void handleMouseMovement(GLFWwindow* window, double xpos, double ypos) {
	int current_width, current_height;
    glfwGetFramebufferSize(window, &current_width, &current_height);//dynamic width and height
	static float lastX = current_width / 2.0f;
    static float lastY = current_height / 2.0f;
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
  //drag logic
	if (app->panelDragActive && app->panel->dragging) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		glm::vec3 camPos = app->firstPersonView ? app->firstPersonCamera.Position : app->globalCamera.Position;
		glm::mat4 view = app->firstPersonView ? app->firstPersonCamera.GetViewMatrix() : app->globalCamera.GetViewMatrix();

		float aspect_ratio = static_cast<float>(current_width) / current_height;
        glm::mat4 proj = glm::perspective(
            glm::radians(app->firstPersonView ? 50.0f : app->globalCamera.Zoom),
            aspect_ratio, 0.1f, 200.0f);

		glm::vec3 rayDir = getPickingRay(xpos, ypos, current_width, current_height, view, proj, camPos);

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
// Handle mouse clicks for picking elements in the UI panel or dragging the panel
void handleMouseClick(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS && app->panel->visible) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            
            int current_width, current_height;
            glfwGetFramebufferSize(window, &current_width, &current_height); //dynamic width and height
            // for accurate ray casting
            glm::vec3 camPos;
            glm::mat4 view;
            glm::mat4 proj;
            // aspect ratio 
            float aspect_ratio = static_cast<float>(current_width) / current_height; 
            
            if (app->firstPersonView) {
                camPos = app->firstPersonCamera.Position;
                view = app->firstPersonCamera.GetViewMatrix();
                proj = glm::perspective(glm::radians(50.0f), aspect_ratio, 0.1f, 200.0f);
            } else {
                camPos = app->globalCamera.Position;
                view = app->globalCamera.GetViewMatrix();
                proj = glm::perspective(glm::radians(app->globalCamera.Zoom), aspect_ratio, 0.1f, 200.0f);
            }
            
            // return rayDir from the camera position to the mouse position
            glm::vec3 rayDir = getPickingRay(xpos, ypos, current_width, current_height, view, proj, camPos);
            // start position camera and direction of the ray -> determine intersection with the panel
            int i = app->panel->pickElement(camPos, rayDir);
            if (i == -1) { // no element clicked but inside the panel
                // Panel drag logic
                float dotNormalRay = glm::dot(app->panel->normal, rayDir);
                if (fabs(dotNormalRay) > 1e-6f) {
                    float t = glm::dot(app->panel->pos - camPos, app->panel->normal) / dotNormalRay;
                    if (t > 0) { 
                        glm::vec3 hit = camPos + t * rayDir; 
                        app->panel->startDrag(hit); 
                        app->panelDragActive = true;
                    }
                }
            } else {
                app->panel->processClick(i);
            }
        }
        if (action == GLFW_RELEASE) {
            app->panel->stopDrag();
            app->panelDragActive = false;
        }
    }
}
// method to get the ray 
//ray casting: getPickingRay -> UIPanel::pickElement -> UIPanel::contains -> UIPanel::processClick
glm::vec3 getPickingRay(double mouseX, double mouseY, int width, int height, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos) {
    
    printf("Mouse: %.2f, %.2f | Viewport: %d, %d\n", mouseX, mouseY, width, height);
    // mouse coord to ndc coords 
    float x = (2.0f * (float)mouseX) / (float)width - 1.0f;
    float y = 1.0f - (2.0f * (float)mouseY) / (float)height;
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);
    glm::mat4 invViewProj = glm::inverse(proj * view);
    glm::vec4 rayWorld = invViewProj * rayClip;
    if (abs(rayWorld.w) > 1e-6f) {
        rayWorld /= rayWorld.w;
    }
    
    // Convert to world coords
    glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld) - camPos);
    // ray direction 
    return rayDir;
}
