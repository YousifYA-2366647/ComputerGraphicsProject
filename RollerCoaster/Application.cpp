#include "Application.h"

// Create a window with the given width and height
Application::Application(): panel(nullptr) {
	window = glfwCreateWindow(WIDTH, HEIGHT, "Roller Coaster", NULL, NULL);

	if (window == NULL) {
		// Exit if the window couldn't be created
		std::cout << "Couldn't create a GLFW window... Terminating." << std::endl;
		glfwTerminate();
		std::exit(-1);
	}


	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		// Exit if GLAD couldn't be initialized
		std::cout << "Couldn't initialize GLAD... Terminating." << std::endl;
		glfwTerminate();
		std::exit(-2);
	}
}

Application::~Application() {
	delete panel;
}

// Runs a loop that keeps rendering the given window
void Application::runWindow() {
	// Create a shader for the curves
	Shader* panelShader = new Shader("panelVertexShader.vert", "panelFragmentShader.frag");

	panel = new UIPanel(glm::vec3(0, 2, 0), glm::vec2(1.5f, 1.0f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	panel->visible = true;
	panel->elements.push_back(new UIButton(glm::vec2(0.1f, 0.7f), glm::vec2(0.8f, 0.2f), [this]() {
		firstPersonView = !firstPersonView;
		}));

	panel->elements.push_back(new UIButton(glm::vec2(0.1f, 0.4f), glm::vec2(0.15f, 0.15f), [this]() {
		speed = std::max(0.1f, speed - 0.5f);
		}));

	panel->elements.push_back(new UIButton(glm::vec2(0.3f, 0.4f), glm::vec2(0.15f, 0.15f), [this]() {
		speed = std::min(10.0f, speed + 0.5f);

		}));

	/// Create bezier curves
	BezierCurve* upperCurve = new BezierCurve(upperCurvePoints);

	BezierCurve* lowerCurve = new BezierCurve(lowerCurvePoints);

	// Create cart model
	Cart* cartObject = new Cart(std::string("model/rollerCoasterModel.glb"));

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);

	upperCurve->setView(view);
	upperCurve->setProjection(projection);
	lowerCurve->setView(view);
	lowerCurve->setProjection(projection);

	cartObject->setView(view);
	cartObject->setProjection(projection);

	// Enable depth test to be able to render things in front of others
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -1.0f);

	// Define 
	float currentTime = glfwGetTime();
	lastFrameTime = currentTime;

	bool onUpper = true;

	float distanceAlongCurve = 0.0f;
	float upperCurveLength = upperCurve->lookupTable.back().arcLength;
	float lowerCurveLength = lowerCurve->lookupTable.back().arcLength;

	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {
		currentTime = glfwGetTime();
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		// Process the use input
		processInput(deltaTime);

		// Give the window a background color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		upperCurve->Draw();
		lowerCurve->Draw();

		// Update the distance along the current curve
		distanceAlongCurve += speed * deltaTime;

		if (onUpper && distanceAlongCurve >= upperCurveLength) {
			onUpper = false;
			distanceAlongCurve = 0.0f;
		}
		else if (!onUpper && distanceAlongCurve >= lowerCurveLength) {
			onUpper = true;
			distanceAlongCurve = 0.0f;
		}

		// Choose the right lookup table
		BezierCurve* currentCurve = onUpper ? upperCurve : lowerCurve;

		// CAMERA SWITCHING LOGIC
		glm::mat4 view;
		if (firstPersonView) {
			firstPersonCamera.Position = cartObject->getPosition() + glm::vec3(0, 1, 0);
			firstPersonCamera.Front = glm::normalize(cartObject->getDirection());
			view = firstPersonCamera.GetViewMatrix();
		}
		else { view = globalCamera.GetViewMatrix(); }

		glm::mat4 projection;
		if (!firstPersonView) {
			projection = glm::perspective(glm::radians(globalCamera.Zoom), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);
		}
		else {
			projection = glm::perspective(glm::radians(50.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);
		}


		upperCurve->setView(view);
		upperCurve->setProjection(projection);
		lowerCurve->setView(view);
		lowerCurve->setProjection(projection);
		cartObject->setView(view);
		cartObject->setProjection(projection);


		// Move and draw the cart
		cartObject->Move(distanceAlongCurve, *currentCurve);
		cartObject->Draw();
		if (panel->visible) {
			panel->draw(*panelShader, view, projection);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete upperCurve;
	delete lowerCurve;
	delete cartObject;
	delete panelShader;
}

// Process user input
void Application::processInput(float deltaTime) {
	if (glfwGetKey(window, quitKey) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

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
			globalCamera.ProcessKeyboard(moveForward, deltaTime);
		if (glfwGetKey(window, moveBackward) == GLFW_PRESS)
			globalCamera.ProcessKeyboard(moveBackward, deltaTime);
		if (glfwGetKey(window, moveLeft) == GLFW_PRESS)
			globalCamera.ProcessKeyboard(moveLeft, deltaTime);
		if (glfwGetKey(window, moveRight) == GLFW_PRESS)
			globalCamera.ProcessKeyboard(moveRight, deltaTime);
		if (glfwGetKey(window, moveUp) == GLFW_PRESS)
			globalCamera.ProcessKeyboard(moveUp, deltaTime);
		if (glfwGetKey(window, moveDown) == GLFW_PRESS)
			globalCamera.ProcessKeyboard(moveDown, deltaTime);
	}
}

void Application::setCursorPosCallback(GLFWcursorposfun callback) {
	glfwSetCursorPosCallback(window, callback);
}

void Application::setInputMode(int mode, int value) {
	glfwSetInputMode(window, mode, value);
}

void Application::setFramebufferSizeCallback(GLFWframebuffersizefun callback) {
	glfwSetFramebufferSizeCallback(window, callback);
}

void Application::setMouseButtonCallback(GLFWmousebuttonfun callback) {
	glfwSetMouseButtonCallback(window, callback);
}

void Application::setScrollCallback(GLFWscrollfun callback) {
	glfwSetScrollCallback(window, callback);
}