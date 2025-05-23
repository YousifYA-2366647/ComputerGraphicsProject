#include "Application.h"


void resizedWindow(GLFWwindow* window, int width, int height);

// Create a window with the given width and height
Application::Application() {
	window = glfwCreateWindow(WIDTH, HEIGHT, "Roller Coaster", NULL, NULL);

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

	runWindow();
}

// Runs a loop that keeps rendering the given window
void Application::runWindow() {
	// Create a shader for the curves
	Shader* curveShader = new Shader("vertexShader.vert", "fragmentShader.frag");

	/// Create bezier curves
	BezierCurve* upperCurve = new BezierCurve(upperCurvePoints);

	BezierCurve* lowerCurve = new BezierCurve(lowerCurvePoints);

	// Create cart model
	Cart* cartObject = new Cart(std::string("model/rollerCoasterModel.glb"));

	// Setup the matrices needed to go 3D
	curveShader->use();

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);

	curveShader->setMat4("view", view);
	curveShader->setMat4("projection", projection);

	cartObject->setView(view);
	cartObject->setProjection(projection);

	// Enable depth test to be able to render things in front of others
	glEnable(GL_DEPTH_TEST);

	// Define 
	float currentTime = glfwGetTime();
	lastFrameTime = currentTime;

	bool onUpper = true;

	float speed = 10.0f; // units per second
	float distanceAlongCurve = 0.0f;
	float upperCurveLength = upperCurve->lookupTable.back().arcLength;
	float lowerCurveLength = lowerCurve->lookupTable.back().arcLength;

	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {
		currentTime = glfwGetTime();
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		// Process the use input
		processInput();

		// Give the window a background color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		upperCurve->Draw(*curveShader);
		lowerCurve->Draw(*curveShader);

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

		// Move and draw the cart
		cartObject->Move(distanceAlongCurve, *currentCurve);
		cartObject->Draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete curveShader;
	delete upperCurve;
	delete lowerCurve;
	delete cartObject;
}

// Resize the viewport along with the window whenever the user resizes the window
void resizedWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// Process user input
void Application::processInput() {
	if (glfwGetKey(window, quitKey) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}