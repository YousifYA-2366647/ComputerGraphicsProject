#include "Application.h"

// Create a window with the given width and height
Application::Application() : panel(nullptr)
{
	window = glfwCreateWindow(WIDTH, HEIGHT, "Roller Coaster", NULL, NULL);

	if (window == NULL)
	{
		// Exit if the window couldn't be created
		std::cout << "Couldn't create a GLFW window... Terminating." << std::endl;
		glfwTerminate();
		std::exit(-1);
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		// Exit if GLAD couldn't be initialized
		std::cout << "Couldn't initialize GLAD... Terminating." << std::endl;
		glfwTerminate();
		std::exit(-2);
	}
    lightManager.addLight(Light(
        glm::vec3(-4.0f, -13.0f, -40.0f),   // positie 
        glm::vec3(0.0f, 1.0f, 0.0f),   // witte kleur
        1.0f,                          // constant attenuatie
        0.09f,                         // lineaire attenuatie
        0.032f                         // kwadratische attenuatie (attenuatie is licht verzwakking naar mate afstand)
    ));
    
    lightManager.addLight(Light(
        glm::vec3(-4.0f, -10.0f, -38.0f), // positie 
        glm::vec3(1.0f, 0.6f, 0.2f),   // oranje kleur
        1.0f,                          // constant attenuatie
        0.09f,                        // lineaire attenuatie
        0.032f                        // kwadratische attenuatie
    ));
    
    lightManager.addLight(Light(
        glm::vec3(-1.0f, 3.0f, -10.0f), // positie bij de camera
        glm::vec3(1.0f, 1.0f, 1.0f),    // witte kleur
        1.0f,                           // constant attenuatie
        0.09f,                          // lineaire attenuatie
        0.032f                          // kwadratische attenuatie
    ));
	
	
}

Application::~Application()
{
	delete panel;
	
}

// Runs a loop that keeps rendering the given window
void Application::runWindow()
{
	// Create a shader for the curves
	Shader *panelShader = new Shader("panelVertexShader.vert", "panelFragmentShader.frag");
    lightManager.initialize();
	panel = new UIPanel(glm::vec3(0, 2, 0), glm::vec2(1.5f, 1.0f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	panel->visible = true;
	panel->elements.push_back(new UIButton(glm::vec2(0.1f, 0.7f), glm::vec2(0.8f, 0.2f), [this]()
										   { firstPersonView = !firstPersonView; }));

	panel->elements.push_back(new UIButton(glm::vec2(0.1f, 0.4f), glm::vec2(0.15f, 0.15f), [this]()
										   { speed = std::max(0.1f, speed - 0.5f); }));

	panel->elements.push_back(new UIButton(glm::vec2(0.3f, 0.4f), glm::vec2(0.15f, 0.15f), [this]() {
		speed = std::min(10.0f, speed + 0.5f);

		}));

	/// Create bezier curves
	BezierCurve *upperCurve = new BezierCurve(upperCurvePoints);

	BezierCurve *lowerCurve = new BezierCurve(lowerCurvePoints);

	// Create cart model
	Cart *cartObject = new Cart(std::string("model/rollerCoasterModel.glb"));

	Terrain* terrainObject = new Terrain();

	SkyBox* skybox = new SkyBox();

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 200.0f);

	upperCurve->setView(view);
	upperCurve->setProjection(projection);
	lowerCurve->setView(view);
	lowerCurve->setProjection(projection);

	cartObject->setView(view);
	cartObject->setProjection(projection);
	terrainObject->setView(view);
	terrainObject->setProjection(projection);

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
	glm::vec3 cameraPos = firstPersonView ? firstPersonCamera.Position : globalCamera.Position;
	cartObject->getShader()->use();
	cartObject->getShader()->setVec3("viewPos", cameraPos);

	// === CREATE BLUR EFFECT ===
	Convolution* convolutor = new Convolution(WIDTH, HEIGHT); // Your blur renderer class

	// Main rendering loop
	while (!glfwWindowShouldClose(window))
	{
		currentTime = glfwGetTime();
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		// Process the use input
		processInput(deltaTime);

		// Give the window a background color
		convolutor->bindBuffer();
		glEnable(GL_DEPTH_TEST); // Enable depth test again
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		upperCurve->Draw();
		lowerCurve->Draw();

		terrainObject->Draw();

		// Update the distance along the current curve
		distanceAlongCurve += speed * deltaTime;

		if (onUpper && distanceAlongCurve >= upperCurveLength)
		{
			onUpper = false;
			distanceAlongCurve = 0.0f;
		}
		else if (!onUpper && distanceAlongCurve >= lowerCurveLength)
		{
			onUpper = true;
			distanceAlongCurve = 0.0f;
		}

		// Choose the right lookup table
		BezierCurve *currentCurve = onUpper ? upperCurve : lowerCurve;

		// CAMERA SWITCHING LOGIC
		glm::mat4 view;
		glm::vec3 cameraPos;
		if (firstPersonView)
		{
			firstPersonCamera.Position = cartObject->getPosition() + glm::vec3(0, 0.45f, 0);

			if (!firstPersonLookingAround)
			{
				// Gebruik de omgekeerde richting van de cart
				glm::vec3 cartDir = -glm::normalize(cartObject->getDirection() * -1.0f);

				firstPersonCamera.Roty = glm::degrees(atan2(cartDir.z, cartDir.x));
				firstPersonCamera.Pitch = glm::degrees(asin(cartDir.y));

				firstPersonCamera.updateCameraVectors();

				savedFirstPersonFront = firstPersonCamera.Front;
			}
            cameraPos = firstPersonCamera.Position;
			wasPriorFirstPerson = true;
			view = firstPersonCamera.GetViewMatrix();
		}
		else
		{
			cameraPos = globalCamera.Position;
			view = globalCamera.GetViewMatrix();
			wasPriorFirstPerson = false;
			firstPersonLookingAround = false;
		}

		glm::mat4 projection;
		if (!firstPersonView) {
			projection = glm::perspective(glm::radians(globalCamera.Zoom), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 200.0f);
		}
		else {
			projection = glm::perspective(glm::radians(50.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 200.0f);
		}

		upperCurve->setView(view);
		upperCurve->setProjection(projection);
		lowerCurve->setView(view);
		lowerCurve->setProjection(projection);
		cartObject->setView(view);
		cartObject->setProjection(projection);
		terrainObject->setView(view);
		terrainObject->setProjection(projection);

		lightManager.applyLighting(cartObject->getShader(), cameraPos);
		std::vector<Shader*> shaders = terrainObject->getBuildingShaders();
		for (Shader* shader : shaders) {
			lightManager.applyLighting(shader, cameraPos);
		}

		

		// Move and draw the cart
		cartObject->Move(distanceAlongCurve, *currentCurve);
		cartObject->Draw();
		lightManager.drawLights(view, projection);

		if (panel->visible)
		{
			panel->draw(*panelShader, view, projection);
		}

		skybox->Draw(view, projection);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_DEPTH_TEST);
		if (blurIntensity) {
			convolutor->Blur(blurIntensity);
		}
		else if (edgeDetectIntensity) {
			convolutor->EdgeDetect(edgeDetectIntensity);
		}
		else if (invertIntensity) {
			convolutor->Inverse(invertIntensity);
		}
		else if (grayIntensity) {
			convolutor->GrayScale(grayIntensity);
		}
		else if (sharpenIntensity) {
			convolutor->Sharpen(sharpenIntensity);
		}
		else {
			convolutor->Blur(0.0f);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete upperCurve;
	delete lowerCurve;
	delete cartObject;
	delete panelShader;
	delete terrainObject;
	delete skybox;
	delete convolutor;
}

// Process user input
void Application::processInput(float deltaTime)
{
	static bool effectKeyPressed = false;
	bool edgeDetectPressed = glfwGetKey(window, edgeKey) == GLFW_PRESS;
	bool blurPressed = glfwGetKey(window, blurKey) == GLFW_PRESS;
	bool invertPressed = glfwGetKey(window, invertKey) == GLFW_PRESS;
	bool grayPressed = glfwGetKey(window, grayScaleKey) == GLFW_PRESS;
	bool sharpenPressed = glfwGetKey(window, sharpenKey) == GLFW_PRESS;
	if (glfwGetKey(window, quitKey) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (blurPressed && !effectKeyPressed) {
		blurIntensity = (blurIntensity == 0.0f) ? 10.0f : 0.0f;
	}
	if (edgeDetectPressed && !effectKeyPressed) {
		edgeDetectIntensity = (edgeDetectIntensity == 0.0f) ? 10.0f : 0.0f;
	}
	if (invertPressed && !effectKeyPressed) {
		invertIntensity = (invertIntensity == 0.0f) ? WIDTH : 0.0f;
	}
	if (grayPressed && !effectKeyPressed) {
		grayIntensity = (grayIntensity == 0.0f) ? WIDTH : 0.0f;
	}
	if (sharpenPressed && !effectKeyPressed) {
		sharpenIntensity = (sharpenIntensity == 0.0f) ? 10.0f : 0.0f;
	}
	effectKeyPressed = edgeDetectPressed || blurPressed || invertPressed || grayPressed || sharpenPressed;

	static bool lastPanelState = false;
	bool panelPressed = glfwGetKey(window, togglePanelKey) == GLFW_PRESS;
	lastPanelState = panelPressed;
	static bool lastSwitchState = false;
	bool switchPressed = glfwGetKey(window, switchPOV) == GLFW_PRESS;
	if (switchPressed && !lastSwitchState)
	{
		firstPersonView = !firstPersonView;
	}
	lastSwitchState = switchPressed;
	if (panelDragActive && panel->dragging)
	{
		float depthSpeed = 5.0f * deltaTime;

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			panel->dragDepth(-depthSpeed);

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			panel->dragDepth(depthSpeed);
	}

	if (!firstPersonView)
	{
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
		if (glfwGetKey(window, moveDown) == GLFW_PRESS)
			globalCamera.ProcessKeyboard(moveDown, deltaTime);
	}
}

void Application::setCursorPosCallback(GLFWcursorposfun callback)
{
	glfwSetCursorPosCallback(window, callback);
}

void Application::setInputMode(int mode, int value)
{
	glfwSetInputMode(window, mode, value);
}

void Application::setFramebufferSizeCallback(GLFWframebuffersizefun callback)
{
	glfwSetFramebufferSizeCallback(window, callback);
}

void Application::setMouseButtonCallback(GLFWmousebuttonfun callback)
{
	glfwSetMouseButtonCallback(window, callback);
}

void Application::setScrollCallback(GLFWscrollfun callback)
{
	glfwSetScrollCallback(window, callback);
}

