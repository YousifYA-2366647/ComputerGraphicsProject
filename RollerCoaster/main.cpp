#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Application.h"
#include <iostream>
#include <vector>

#include "Shader.h"
#include "BezierCurve.h"
#include "vertex.h"
#include "Cart.h"
#include "Camera.h"
#include "UIPanel.h"

UIPanel* panel;
float speed = 2.0f; 

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
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

bool showPanel = false;
unsigned int togglePanelKey = GLFW_KEY_TAB; 
bool panelDragActive = false;



// bezier curve data
std::vector<Vertex> upperCurvePoints = {
	// Position                                   // Color
	{glm::vec3(0.0f, 0.0f, 0.0f),                 glm::vec3(1.0f, 1.0f, 1.0f)},  // U0 (Start of upper curve)
	{glm::vec3(2.0f, 1.0f, 0.8f),                 glm::vec3(1.0f, 0.5f, 0.0f)},  // U1 (MODIFIED for C1 continuity)
	{glm::vec3(2.5f, 2.0f, 0.2f),                 glm::vec3(0.5f, 1.0f, 0.0f)},  // U2
	{glm::vec3(3.0f, 1.0f, -0.8f),                glm::vec3(0.0f, 0.5f, 1.0f)},  // U3
	{glm::vec3(1.0f, -0.5f, 0.5f),                glm::vec3(1.0f, 0.0f, 0.5f)}   // U4 (End of upper curve, connects to start of lower)
};

std::vector<Vertex> lowerCurvePoints = {
	// Position                                   // Color
	{glm::vec3(1.0f, -0.5f, 0.5f),				  glm::vec3(1.0f, 0.0f, 0.5f)},  // LP0 (Start of lower, connected to U4.Position)
	{glm::vec3(-1.0f, -2.0f, 1.8f),                glm::vec3(1.0f, 0.5f, 0.0f)},  // LP1 (MODIFIED for C1 continuity)
	{glm::vec3(-0.5f, -2.0f, 0.2f),               glm::vec3(0.5f, 1.0f, 0.0f)},  // LP2
	{glm::vec3(-2.0f, -1.0f, -0.8f),              glm::vec3(0.0f, 0.5f, 1.0f)},  // LP3
	{glm::vec3(0.0f, 0.0f, 0.0f),				  glm::vec3(1.0f, 1.0f, 1.0f)}   // LP4 (End of lower, connected to U0.Position)
};

// Global/free camera startposities
Camera globalCamera(glm::vec3(0.0f, 2.0f, 10.0f), glm::vec3(0, 1, 0), -90.0f, 0.0f);
Camera firstPersonCamera(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0, 1, 0), -90.0f, 0.0f);



// Flag om te switchen tussen views
bool firstPersonView = true;

/// functions
void initializeGLFW();
GLFWwindow* createGLFWwindow(int width, int height);
void runWindow(GLFWwindow* window);
void resizedWindow(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float deltaTime);
void handleMouseMovement(GLFWwindow* window, double xpos, double ypos);
void handleScrollInput(GLFWwindow* window, double xoffset, double yoffset);
void handleMouseClick(GLFWwindow* window, int button, int action, int mods);
glm::vec3 getPickingRay(double mouseX, double mouseY, int width, int height, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos);

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

// Create a window with the given width and height
GLFWwindow* createGLFWwindow(int width, int height) {
	GLFWwindow* window = glfwCreateWindow(width, height, "Roller Coaster", NULL, NULL);

	if (window == NULL) {
		// Exit if the window couldn't be created
		std::cout << "Couldn't create a GLFW window... Terminating." << std::endl;
		glfwTerminate();
		std::exit(-1);
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
	glfwSetCursorPosCallback(window, handleMouseMovement);
	glfwSetScrollCallback(window, handleScrollInput);

	glfwSetMouseButtonCallback(window, handleMouseClick);
	glfwSetCursorPosCallback(window, handleMouseMovement);



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
	Shader* panelShader = new Shader("panelVertexShader.vert", "panelFragmentShader.frag");
	
	panel = new UIPanel(glm::vec3(0, 2, 0), glm::vec2(1.5f, 1.0f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	panel->visible = true;
	panel->elements.push_back(new UIButton(glm::vec2(0.1f, 0.7f), glm::vec2(0.8f, 0.2f), []() {
		firstPersonView = !firstPersonView;
		}));

	panel->elements.push_back(new UIButton(glm::vec2(0.1f, 0.4f), glm::vec2(0.15f, 0.15f), []() {
		speed = std::max(0.1f, speed - 0.5f);
		}));

	panel->elements.push_back(new UIButton(glm::vec2(0.3f, 0.4f), glm::vec2(0.15f, 0.15f), []() {
		speed = std::min(10.0f, speed + 0.5f);

		}));

	/// calculate bezier curve
	BezierCurve* upperCurve = new BezierCurve(upperCurvePoints);

	BezierCurve* lowerCurve = new BezierCurve(lowerCurvePoints);

	Cart* cartObject = new Cart(std::string("model/rollerCoasterModel.glb"));

	// Tell the program to use the shader's program
	curveShader->use();

	glm::mat4 curveModel = glm::mat4(1.0f);
	curveModel = glm::rotate(curveModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	curveModel = glm::scale(curveModel, glm::vec3(6.0f));
	curveModel = glm::translate(curveModel, glm::vec3(-2.0f, -2.0f, 0.0f));

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);

	curveShader->setMat4("model", curveModel);
	curveShader->setMat4("view", view);
	curveShader->setMat4("projection", projection);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -1.0f);
	


	float currentTime = glfwGetTime();
	lastFrameTime = currentTime;

	bool onUpper = true;

	
	float distanceAlongCurve = 0.0f;
	float upperCurveLength = upperCurve->lookupTable.back().arcLength;
	float lowerCurveLength = lowerCurve->lookupTable.back().arcLength;

	glm::vec3 prevDirection = glm::vec3(0.0f, 0.0f, 1.0f);

	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {
		currentTime = glfwGetTime();
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		processInput(window, deltaTime);

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

		// Find the position at this effective distance using linear search
		glm::vec3 position = table.back().arcVertex.Position; // fallback
		glm::vec3 direction = prevDirection;
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
				glm::vec3 interpPos = glm::mix(a.arcVertex.Position, b.arcVertex.Position, f);

				glm::vec4 worldPos = curveModel * glm::vec4(interpPos, 1.0f);
				position = glm::vec3(worldPos);

				float t = glm::mix(a.t, b.t, f);
				direction = currentCurve->calculateBezierDerivative(t);

				direction = glm::normalize(glm::mat3(curveModel) * direction);
				break;
			}
		}
		
		float rotationSmoothness = 0.1f;
		glm::vec3 smoothedDirection = glm::normalize(glm::mix(prevDirection, direction, rotationSmoothness));

		prevDirection = smoothedDirection;


		// CAMERA SWITCHING LOGIC
		glm::mat4 view;
		if (firstPersonView) {
			firstPersonCamera.Position = position + glm::vec3(0, 1, 0);
			firstPersonCamera.Front = glm::normalize(direction);
			view = firstPersonCamera.GetViewMatrix();
		}
		else {view = globalCamera.GetViewMatrix();}

		glm::mat4 projection;
		if (!firstPersonView) {
			projection = glm::perspective(glm::radians(globalCamera.Zoom), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);
		}
		else {
			projection = glm::perspective(glm::radians(50.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);
		}

		curveShader->use();
		curveShader->setMat4("view", view);
		curveShader->setMat4("projection", projection);
		modelShader->use();
		modelShader->setMat4("view", view);
		modelShader->setMat4("projection", projection);

		cartObject->Move(position, direction);
		modelShader->use();
		modelShader->setMat4("model", cartObject->getModel());
		cartObject->Draw(*modelShader);
		if (panel->visible) {
			panel->draw(*panelShader, view, projection);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
		


	}


	delete curveShader;
	delete upperCurve;
	delete lowerCurve;
	delete cartObject;
	delete panelShader;
}

// Resize the viewport along with the window whenever the user resizes the window
void resizedWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, float deltaTime) {
	static bool lastPanelState = false;
	bool panelPressed = glfwGetKey(window, togglePanelKey) == GLFW_PRESS;
	lastPanelState = panelPressed;
	static bool lastSwitchState = false;
	bool switchPressed = glfwGetKey(window, switchPOV) == GLFW_PRESS;
	if (switchPressed && !lastSwitchState) {
		firstPersonView = !firstPersonView;
	}
	lastSwitchState = switchPressed;
	if (panelDragActive && panel->dragging) {
		float depthSpeed = 5.0f * deltaTime; 

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			panel->dragDepth(-depthSpeed);  

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			panel->dragDepth(depthSpeed); 
	}

	if (!firstPersonView) {
		if (glfwGetKey(window, moveForward) == GLFW_PRESS)
			globalCamera.ProcessKeyboard(GLFW_KEY_W, deltaTime);
		if (glfwGetKey(window, moveBackward) == GLFW_PRESS)
			globalCamera.ProcessKeyboard(GLFW_KEY_S, deltaTime);
		if (glfwGetKey(window, moveLeft) == GLFW_PRESS)
			globalCamera.ProcessKeyboard(GLFW_KEY_A, deltaTime);
		if (glfwGetKey(window, moveRight) == GLFW_PRESS)
			globalCamera.ProcessKeyboard(GLFW_KEY_D, deltaTime);
	}
}
void handleMouseMovement(GLFWwindow* window, double xpos, double ypos) {
	static float lastX = WIDTH / 2.0f;
	static float lastY = HEIGHT / 2.0f;
	static bool firstMouse = true;

	static bool wasRightPressed = false;
	bool rightPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	if (rightPressed && !wasRightPressed) {
		firstMouse = true; // Reset bij nieuwe klik
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

	if (firstPersonView) {
		firstPersonCamera.ProcessMouseMovement(xoffset, yoffset);
	}
	else {
		globalCamera.ProcessMouseMovement(xoffset, yoffset);
	}
	if (panelDragActive && panel->dragging) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		glm::vec3 camPos = firstPersonView ? firstPersonCamera.Position : globalCamera.Position;

		glm::mat4 view = firstPersonView ? firstPersonCamera.GetViewMatrix() : globalCamera.GetViewMatrix();

		glm::mat4 proj = glm::perspective(
			glm::radians(firstPersonView ? 50.0f : globalCamera.Zoom),
			float(WIDTH) / HEIGHT, 0.1f, 100.0f);

		glm::vec3 rayDir = getPickingRay(xpos, ypos, WIDTH, HEIGHT, view, proj, camPos);

		float dotNormalRay = glm::dot(panel->normal, rayDir);
		if (fabs(dotNormalRay) > 1e-6f) {
			float t = glm::dot(panel->pos - camPos, panel->normal) / dotNormalRay;
			if (t > 0) {
				glm::vec3 hit = camPos + t * rayDir;
				panel->updateDrag(hit);
			}
		}
	}

}
void handleScrollInput(GLFWwindow* window, double xoffset, double yoffset) {
	if (!firstPersonView) {
		globalCamera.ProcessMouseScroll((float)yoffset);
	}
	else {
		firstPersonCamera.ProcessMouseScroll((float)yoffset);
	}
}
void handleMouseClick(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS && panel->visible) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			glm::vec3 camPos = globalCamera.Position;
			glm::mat4 view = globalCamera.GetViewMatrix();
			glm::mat4 proj = glm::perspective(glm::radians(globalCamera.Zoom), float(WIDTH) / HEIGHT, 0.1f, 100.0f);
			glm::vec3 rayDir = getPickingRay(xpos, ypos, WIDTH, HEIGHT, view, proj, camPos);

			int i = panel->pickElement(camPos, rayDir);
			if (i == -1) {
				// Ray-plane intersection voor het panel
				float dotNormalRay = glm::dot(panel->normal, rayDir);
				if (fabs(dotNormalRay) > 1e-6f) {
					float t = glm::dot(panel->pos - camPos, panel->normal) / dotNormalRay;
					if (t > 0) { glm::vec3 hit = camPos + t * rayDir; panel->startDrag(hit); panelDragActive = true;
					}
				}
			}
			else {
				panel->processClick(i);
			}
		}
		if (action == GLFW_RELEASE) {
			panel->stopDrag();
			panelDragActive = false;
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
