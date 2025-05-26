#include "Application.h"

// Create a window with the given width and height
Application::Application() : panel(nullptr), chromaKeyPictureFrame(nullptr), projection(glm::mat4(1.0f)), savedFirstPersonFront(glm::vec3(1.0f)), view(glm::mat4(1.0f))
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

	// Calculate window size
	int actual_width, actual_height;
    glfwGetFramebufferSize(window, &actual_width, &actual_height);
	printf("Framebuffer size: %d x %d\n", actual_width, actual_height);
    glViewport(0, 0, actual_width, actual_height);
	printf("Viewport set to: %d x %d\n", actual_width, actual_height);
    current_width = actual_width;
    current_height = actual_height;
  
	// Add lights

	struct LightInfo {
		glm::vec3 Position;
		glm::vec3 Color;
		bool shouldFlip;
		float constantAttentuation;
		float linearAttentuation;
		float quadraticAttentuation;
	};

	std::vector<LightInfo> information = {
		// Green light
		{
			glm::vec3(-4.0f, -16.0f, -46.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			false,
			1.0f,
			0.09f,
			0.0032f
		},

		// Orange light
		{
			glm::vec3(-4.0f, -16.0f, -33.0f),
			glm::vec3(1.0f, 0.2f, 0.6f),
			true,
			1.0f,
			0.09f,
			0.0032f
		},

		// Purple Light
		{
			glm::vec3(-3.0f, -12.0f, -33.0f),
			glm::vec3(0.3f, 0.2f, 0.8f),
			true,
			1.0f,
			0.09f,
			0.0032f
		},

		// White light
		{
			glm::vec3(-1.0f, 3.0f, -11.2f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			false,
			1.0f,
			0.04f,
			0.002f
		},

		// Red light
		{
			glm::vec3(5.0f, 27.0f, 18.1f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			true,
			1.0f,
			0.07f,
			0.0025f
		},

		// Green light
		{
			glm::vec3(-4.0f, 27.0f, 18.1f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			true,
			1.0f,
			0.04f,
			0.0025f
		},

		// Blue light
		{
			glm::vec3(0.5f, 27.0f, 18.1f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			true,
			1.0f,
			0.07f,
			0.0025f
		},

		// Strong yellow light
		{
			glm::vec3(-10.0f, 68.0f, -11.2f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			false,
			1.0f,
			0.01f,
			0.001f
		},
	};

	for (int i = 0; i < information.size(); i++) {

		lightManager.addLight(Light(
			information[i].Position,						// positie 
			information[i].Color,							// kleur
			information[i].constantAttentuation,			// constant attenuatie
			information[i].linearAttentuation,				// lineaire attenuatie
			information[i].quadraticAttentuation			// kwadratische attenuatie (attenuatie is licht verzwakking naar mate afstand)
		));

		LightHolder* lightHolder = new LightHolder(information[i].Position, information[i].shouldFlip);

		lightHolders.push_back(lightHolder);
	}
}

Application::~Application()
{
	delete panel;
	delete chromaKeyPictureFrame;
	for (int i = 0; i < lightHolders.size(); i++) {
		delete lightHolders[i];
	}
	lightHolders.clear();
}

// Runs a loop that keeps rendering the given window
void Application::runWindow()
{
	int current_width, current_height;
	glfwGetFramebufferSize(window, &current_width, &current_height);
	// Create all necessary objects for the world
	// Create UI panel
	Shader *panelShader = new Shader("panelVertexShader.vert", "panelFragmentShader.frag");
	lightManager.initialize();
	chromaKeyPictureFrame = new ChromaKeyPictureFrame("model/camera_frame.png");
	panel = new UIPanel(glm::vec3(0, 2, 0), glm::vec2(3.0f, 2.0f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	panel->visible = true;
	panel->elements.push_back(new UIButton(glm::vec2(0.1f, 0.7f), glm::vec2(0.8f, 0.25f), [this]()
										   { if (showPanel) firstPersonView = !firstPersonView; }));

	panel->elements.push_back(new UIButton(glm::vec2(0.1f, 0.4f), glm::vec2(0.30f, 0.15f), [this]()
										   {  if (showPanel) speed = std::max(0.1f, speed - 0.5f); }));

	panel->elements.push_back(new UIButton(glm::vec2(0.6f, 0.4f), glm::vec2(0.30f, 0.15f), [this]()
										   { if (showPanel) speed = std::min(10.0f, speed + 0.5f); }));
	/// Create bezier curves
	BezierCurve *upperCurve = new BezierCurve(upperCurvePoints);

	BezierCurve *lowerCurve = new BezierCurve(lowerCurvePoints);

	// Create cart model
	Cart *cartObject = new Cart(std::string("model/rollerCoasterModel.glb"));

	// Create Terrain
	Terrain *terrainObject = new Terrain();

	// Create Skybox
	SkyBox *skybox = new SkyBox();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -1.0f);

	float currentTime = glfwGetTime();
	lastFrameTime = currentTime;

	// Variables needed to move the cart
	bool onUpper = true;

	float distanceAlongCurve = 0.0f;
	float upperCurveLength = upperCurve->lookupTable.back().arcLength;
	float lowerCurveLength = lowerCurve->lookupTable.back().arcLength;
	glm::vec3 cameraPos = firstPersonView ? firstPersonCamera.Position : globalCamera.Position;
	cartObject->modelShader->use();
	cartObject->modelShader->setVec3("viewPos", cameraPos);

	// Create convolutor for post processing effects
	Convolution *convolutor = new Convolution(current_width, current_height); 

	// Set projection
	projection = glm::perspective(glm::radians(
		firstPersonView ? 50.0f : globalCamera.Zoom
	), static_cast<float>(current_width) / current_height, 0.1f, 200.0f);

	// Main rendering loop
	while (!glfwWindowShouldClose(window))
	{
		// Get the current size of the window to update the screen quads
		int actual_width, actual_height;
		glfwGetFramebufferSize(window, &actual_width, &actual_height);
		glViewport(0, 0, actual_width, actual_height);

		if (framebufferResized) {
			convolutor->resize(actual_width, actual_height);
			framebufferResized = false;
		}

		// Calculate the time between frames
		glfwGetFramebufferSize(window, &current_width, &current_height);
		currentTime = glfwGetTime();
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		// Process the use input
		processInput(deltaTime);

		// CAMERA SWITCHING LOGIC
		glm::vec3 cameraPos;
		if (firstPersonView)
		{
			firstPersonCamera.Position = cartObject->getPosition() + glm::vec3(0, 0.2f, 0) * cartObject->size;

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

		// Bind the framebuffer that is needed at this moment
		if (bloomBlurIntensity) {
			convolutor->bindHDR();
		}
		else {
			convolutor->bindBuffer();
		}

		// Enable depth test and give the background a color
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set the view and projection of the objects that will be rendered
		upperCurve->setView(view);
		lowerCurve->setView(view);

		cartObject->setView(view);
		terrainObject->setView(view);

		float aspect_ratio = static_cast<float>(current_width) / current_height;
		projection = glm::perspective(glm::radians(
			firstPersonView ? 50.0f : globalCamera.Zoom
		), aspect_ratio, 0.1f, 200.0f);

		upperCurve->setProjection(projection);
		lowerCurve->setProjection(projection);

		cartObject->setProjection(projection);
		terrainObject->setProjection(projection);
		if (panel) panelShader->setMat4("projection", projection);

		for (int i = 0; i < lightHolders.size(); i++) {
			lightHolders[i]->setView(view);
			lightHolders[i]->setProjection(projection);
		}

		// Apply lighting to the objects that should be affected
		lightManager.applyLighting(cartObject->modelShader, cameraPos);
		std::vector<Shader*> shaders = terrainObject->getBuildingShaders();
		for (Shader* shader : shaders) {
			lightManager.applyLighting(shader, cameraPos);
		}
		lightManager.applyLighting(upperCurve->getTrack()->trackShader, cameraPos);
		lightManager.applyLighting(lowerCurve->getTrack()->trackShader, cameraPos);
		for (int i = 0; i < lightHolders.size(); i++) {
			lightManager.applyLighting(lightHolders[i]->getShader(), cameraPos);
		}
		
		// Draw all objects
		upperCurve->Draw();
		lowerCurve->Draw();

		terrainObject->Draw();

		for (int i = 0; i < lightHolders.size(); i++) {
			lightHolders[i]->Draw();
		}

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

		// Move and draw the cart
		cartObject->Move(distanceAlongCurve, *currentCurve);
		cartObject->Draw();
		lightManager.drawLights(view, projection);

		if (showPanel)
		{
			panel->draw(*panelShader, view, projection);
		}

		skybox->Draw(view, projection);
		if (showChromaKey)
		{
			chromaKeyPictureFrame->Draw();
		}

		// Unbind the framebuffer and draw the chosen effect, or none if nothing is chosen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_DEPTH_TEST);
		if (blurIntensity)
		{
			convolutor->Blur(blurIntensity);
		}
		else if (edgeDetectIntensity)
		{
			convolutor->EdgeDetect(edgeDetectIntensity);
		}
		else if (invertIntensity)
		{
			convolutor->Inverse(invertIntensity);
		}
		else if (grayIntensity)
		{
			convolutor->GrayScale(grayIntensity);
		}
		else if (sharpenIntensity)
		{
			convolutor->Sharpen(sharpenIntensity);
		}
		else if (bloomBlurIntensity) {
			convolutor->Bloom(0.2f, bloomBlurIntensity, 3);
		}
		else
		{
			convolutor->Blur(0.0f);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// Clean all pointers up
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
	int current_width, current_height;
    glfwGetFramebufferSize(window, &current_width, &current_height);

	// Process effect keys
	static bool effectKeyPressed = false;
	bool edgeDetectPressed = glfwGetKey(window, edgeKey) == GLFW_PRESS;
	bool blurPressed = glfwGetKey(window, blurKey) == GLFW_PRESS;
	bool invertPressed = glfwGetKey(window, invertKey) == GLFW_PRESS;
	bool grayPressed = glfwGetKey(window, grayScaleKey) == GLFW_PRESS;
	bool sharpenPressed = glfwGetKey(window, sharpenKey) == GLFW_PRESS;
	bool bloomPressed = glfwGetKey(window, bloomKey) == GLFW_PRESS;
	if (glfwGetKey(window, quitKey) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (blurPressed && !effectKeyPressed)
	{
		blurIntensity = (blurIntensity == 0.0f) ? 10.0f : 0.0f;
	}
	if (edgeDetectPressed && !effectKeyPressed)
	{
		edgeDetectIntensity = (edgeDetectIntensity == 0.0f) ? 10.0f : 0.0f;
	}
	 if (invertPressed && !effectKeyPressed) {
        invertIntensity = (invertIntensity == 0.0f) ? current_width : 0.0f;
    }
    if (grayPressed && !effectKeyPressed) {
        grayIntensity = (grayIntensity == 0.0f) ? current_width : 0.0f;
    }
	if (sharpenPressed && !effectKeyPressed)
	{
		sharpenIntensity = (sharpenIntensity == 0.0f) ? 10.0f : 0.0f;
	}
	if (bloomPressed && !effectKeyPressed)
	{
		bloomBlurIntensity = (bloomBlurIntensity == 0.0f) ? 1.0f : 0.0f;
	}
	effectKeyPressed = edgeDetectPressed || blurPressed || invertPressed || grayPressed || sharpenPressed || bloomPressed;

	// Toggle UI panel
	static bool lastPanelState = false;
	bool panelPressed = glfwGetKey(window, togglePanelKey) == GLFW_PRESS;
	if (panelPressed && !lastPanelState) {
		showPanel = !showPanel;
	}
	lastPanelState = panelPressed;

	// Toggle POV
	static bool lastSwitchState = false;
	bool switchPressed = glfwGetKey(window, switchPOV) == GLFW_PRESS;
	if (switchPressed && !lastSwitchState)
	{
		firstPersonView = !firstPersonView;
	}
	lastSwitchState = switchPressed;

	// Drag UI panel around
	if (panelDragActive && panel->dragging)
	{
		float depthSpeed = 5.0f * deltaTime;

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			panel->dragDepth(-depthSpeed);

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			panel->dragDepth(depthSpeed);
	}

	// Toggle chroma keying
	static bool lastChromaKeyState = false;
	bool chromaKeyPressed = glfwGetKey(window, toggleChromaKeyKey) == GLFW_PRESS;
	if (chromaKeyPressed && !lastChromaKeyState)
	{
		showChromaKey = !showChromaKey;
	}
	lastChromaKeyState = chromaKeyPressed;

	// Process movement keys
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
