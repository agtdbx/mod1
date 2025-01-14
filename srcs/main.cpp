#include <fstream>
#include <sstream>

#include <define.hpp>
#include <interfaceDeclaratiom.hpp>
#include <engine/inputs/InputManager.hpp>
#include <engine/render/shader/ShaderManager.hpp>
#include <engine/render/Mesh.hpp>
#include <engine/render/TextureManager.hpp>
#include <engine/render/Camera.hpp>
#include <engine/OpenGLContext.hpp>
#include <model/Terrain.hpp>
#include <model/Button.hpp>
#include <model/Pannel.hpp>
#include <model/Slider.hpp>
#include <model/WaterSimulation.hpp>

static void	events(
				GLFWwindow* window,
				InputManager *inputManager);
static void	computation(
				InputManager *inputManager,
				Camera *camera,
				OpenGLContext *context,
				t_simulationVariable *sVar,
				WaterSimulation	*simulation,
				ShaderManager *shaderManager);
static void	draw(
				GLFWwindow* window,
				Camera *camera,
				Terrain *terrain,
				ShaderManager *shaderManager,
				t_simulationVariable *sVar,
				WaterSimulation	*simulation);



int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Error : Wrong number of argument" << std::endl;
		return (1);
	}

	if (!glfwInit())
	{
		std::cerr << "Error : Opengl init failed" << std::endl;
		return (1);
	}

	Terrain	terrain;

	try
	{
		terrain.loadFromFile(argv[1]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	OpenGLContext	context;

	if (!context.isInitGood())
	{
		glfwTerminate();
		return (1);
	}

	InputManager	inputManager(context.window);
	ShaderManager	shaderManager;
	TextureManager	textureManager;
	WaterSimulation	simulation;
	Camera			camera;

	t_simulationVariable	sVar;

	sVar.fillingIntensity = FILLING_INTENSITY;
	sVar.fillingDelay = FILLING_TIME_BEFORE_NEW_PARTICULE;
	sVar.isFilling = false;
	sVar.isPannelHide = true;
	sVar.isRainning = false;
	sVar.rainIntensity = RAIN_INTENSITY;
	sVar.rainDelay = RAIN_TIME_BEFORE_NEW_PARTICULE;
	sVar.simulation = &simulation;
	sVar.waveHeight = WAVE_HEIGHT;
	sVar.waveThickess = WAVE_THICKNESS;
	sVar.waveVelocity = WAVE_VELOCITY;

	// std::vector<Pannel> pannelVector;
	// bool				isRainning = false;
	// bool				isFilling = false;
	// bool				isPannelHide = false;

	Button::mouse = &inputManager.mouse;
	Slider::mouse = &inputManager.mouse;

	try
	{
		textureManager.addTexture("dirt", "data/textures/dirt.png");

		textureManager.addTexture("rain", "data/textures/rainButton.png");
		textureManager.addTexture("reset", "data/textures/resetButton.png");
		textureManager.addTexture("filling", "data/textures/fillingButton.png");
		textureManager.addTexture("wave", "data/textures/waveButton.png");

		textureManager.addTexture("active", "data/textures/active.png");
		textureManager.addTexture("noTexture", "data/textures/noTexture.png");
		Slider::texture = textureManager.getTexture("noTexture");

		textureManager.addTexture("North", "data/textures/North.png");
		textureManager.addTexture("South", "data/textures/South.png");
		textureManager.addTexture("West", "data/textures/West.png");
		textureManager.addTexture("Est", "data/textures/Est.png");
		textureManager.addTexture("all", "data/textures/all.png");
		textureManager.addTexture("waveThickness", "data/textures/waveThickness.png");
		textureManager.addTexture("waveHeight", "data/textures/waveHeight.png");
		textureManager.addTexture("waveVelocity", "data/textures/waveVelocity.png");

		textureManager.addTexture("rainIntensity", "data/textures/rainIntensity.png");
		textureManager.addTexture("rainDelay", "data/textures/rainDelay.png");

		textureManager.addTexture("fillingIntensity", "data/textures/fillingIntensity.png");
		textureManager.addTexture("fillingDelay", "data/textures/fillingDelay.png");

		shaderManager.addShader("terrain", "data/shaders/terrain/terrain.glslv", "data/shaders/terrain/terrain.glslf");
		shaderManager.loadWaterShaderFiles("data/shaders/water/water.glslv", "data/shaders/water/waterBall.glslf");
		shaderManager.loadMenuShaderFiles("data/shaders/menu.vs", "data/shaders/menu.fs");

		shaderManager.addComputeShader("predictedPositions", "data/shaders/simulation/predictedPositions.glslc");
		shaderManager.addComputeShader("densities", "data/shaders/simulation/densities.glslc");
		shaderManager.addComputeShader("pressure", "data/shaders/simulation/pressure.glslc");
		shaderManager.addComputeShader("updatePositions", "data/shaders/simulation/updatePositions.glslc");
	}
	catch (std::exception &e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
		context.close();
		glfwTerminate();
		return (1);
	}

	sVar.pannelVector.push_back(Pannel(WIN_W, 0.0f, 120, 250, textureManager.getTexture("noTexture"), PANNEL_COLOR));	//main pannel
	sVar.pannelVector.push_back(Pannel(WIN_W + 120, 255.0f, 120, 590, textureManager.getTexture("noTexture"), PANNEL_COLOR));	//wave pannel
	sVar.pannelVector.push_back(Pannel(0 - 240, 0.0f, 120, 310, textureManager.getTexture("noTexture"), PANNEL_COLOR));	//rain pannel
	sVar.pannelVector.push_back(Pannel(0 - 240, 320.0f, 120, 310, textureManager.getTexture("noTexture"), PANNEL_COLOR));	//filling pannel

	sVar.pannelVector[0].addButton(Button(10, 10, 100, 50,moveRainPannel, &sVar.pannelVector[2], textureManager.getTexture("rain")));
	sVar.pannelVector[0][0].setSwitchMode(true);
	sVar.pannelVector[0].addButton(Button(10, 70, 100, 50,moveFillingPannel, &sVar.pannelVector[3], textureManager.getTexture("filling")));
	sVar.pannelVector[0][1].setSwitchMode(true);
	sVar.pannelVector[0].addButton(Button(10, 130, 100, 50,moveWavePannel, &sVar.pannelVector[1], textureManager.getTexture("wave")));
	sVar.pannelVector[0][2].setSwitchMode(true);
	sVar.pannelVector[0].addButton(Button(10, 190, 100, 50,resetPool, &simulation, textureManager.getTexture("reset")));
	// sVar.pannelVector[0].addSlider(Slider(-500, 500, 200, 10, COLOR_29266F, COLOR_2C26E4));
	// sVar.pannelVector[0][0.0f].setValue(0.5);

	sVar.pannelVector[1].addButton(Button(10, 10, 100, 50,generateWaveNorth, &sVar, textureManager.getTexture("North")));
	sVar.pannelVector[1].addButton(Button(10, 70, 100, 50,generateWaveWest, &sVar, textureManager.getTexture("West")));
	sVar.pannelVector[1].addButton(Button(10, 130, 100, 50,generateWaveEst, &sVar, textureManager.getTexture("Est")));
	sVar.pannelVector[1].addButton(Button(10, 190, 100, 50,generateWaveSouth, &sVar, textureManager.getTexture("South")));
	sVar.pannelVector[1].addButton(Button(10, 250, 100, 50,generateWaveAll, &sVar, textureManager.getTexture("all")));
	sVar.pannelVector[1].addButton(Button(10, 310, 100, 50,NULL, NULL, textureManager.getTexture("waveHeight"), PANNEL_COLOR, PANNEL_COLOR));
	sVar.pannelVector[1][5].desactive();
	sVar.pannelVector[1].addSlider(Slider(10, 370, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar.pannelVector[1][0.0f].setValue(0.5);
	sVar.pannelVector[1].addButton(Button(10, 400, 100, 50,NULL, NULL, textureManager.getTexture("waveVelocity"), PANNEL_COLOR, PANNEL_COLOR));
	sVar.pannelVector[1][6].desactive();
	sVar.pannelVector[1].addSlider(Slider(10, 460, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar.pannelVector[1][1.0f].setValue(0.5);
	sVar.pannelVector[1].addButton(Button(10, 490, 100, 50,NULL, NULL, textureManager.getTexture("waveThickness"), PANNEL_COLOR, PANNEL_COLOR));
	sVar.pannelVector[1][7].desactive();
	sVar.pannelVector[1].addSlider(Slider(10, 560, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar.pannelVector[1][2.0f].setValue(0.5);


	sVar.pannelVector[2].addButton(Button(10, 10, 100, 50,changeBoolStatus, &sVar.isRainning, textureManager.getTexture("rain"), PANNEL_COLOR, PANNEL_COLOR));
	sVar.pannelVector[2][0].desactive();
	sVar.pannelVector[2].addButton(Button(10, 70, 100, 50,changeBoolStatus, &sVar.isRainning, textureManager.getTexture("active")));
	sVar.pannelVector[2][1].setSwitchMode(true);
	sVar.pannelVector[2].addButton(Button(10, 130, 100, 50,NULL, NULL, textureManager.getTexture("rainIntensity"), PANNEL_COLOR, PANNEL_COLOR));
	sVar.pannelVector[2][2].desactive();
	sVar.pannelVector[2].addSlider(Slider(10, 190, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar.pannelVector[2][0.0f].setValue(0.5);
	sVar.pannelVector[2].addButton(Button(10, 210, 100, 50,NULL, NULL, textureManager.getTexture("rainDelay"), PANNEL_COLOR, PANNEL_COLOR));
	sVar.pannelVector[2][3].desactive();
	sVar.pannelVector[2].addSlider(Slider(10, 270, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar.pannelVector[2][1.0f].setValue(0.5);

	sVar.pannelVector[3].addButton(Button(10, 10, 100, 50,changeBoolStatus, &sVar.isRainning, textureManager.getTexture("filling"), PANNEL_COLOR, PANNEL_COLOR));
	sVar.pannelVector[3][0].desactive();
	sVar.pannelVector[3].addButton(Button(10, 70, 100, 50,changeBoolStatus, &sVar.isFilling, textureManager.getTexture("active")));
	sVar.pannelVector[3][1].setSwitchMode(true);
	sVar.pannelVector[3].addButton(Button(10, 130, 100, 50,NULL, NULL, textureManager.getTexture("fillingIntensity"), PANNEL_COLOR, PANNEL_COLOR));
	sVar.pannelVector[3][2].desactive();
	sVar.pannelVector[3].addSlider(Slider(10, 190, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar.pannelVector[3][0.0f].setValue(0.5);
	sVar.pannelVector[3].addButton(Button(10, 210, 100, 50,NULL, NULL, textureManager.getTexture("fillingDelay"), PANNEL_COLOR, PANNEL_COLOR));
	sVar.pannelVector[3][3].desactive();
	sVar.pannelVector[3].addSlider(Slider(10, 270, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar.pannelVector[3][1.0f].setValue(0.5);


	// simulation.addWater(glm::vec3(5, 5, 5));
	// int	nbWater[] = {32, 32, 32};
	// glm::vec3	offset(MAP_SIZE / 2 - nbWater[0] / 2, 5, MAP_SIZE / 2 - nbWater[2] / 2);
	// for (int i = 0; i < nbWater[0]; i++)
	// {
	// 	for (int j = 0; j < nbWater[2]; j++)
	// 	{
	// 		for (int k = 0; k < nbWater[1]; k++)
	// 		{
	// 			simulation.addWater(glm::vec3(i, k, j) + offset);
	// 		}
	// 	}
	// }

	// Main loop
	inputManager.mouse.goTo(context.window, WIN_W / 2, WIN_H / 2);
	inputManager.mouse.setVisible(context.window, false);

	while (!glfwWindowShouldClose(context.window))
	{
		events(context.window, &inputManager);

		// Close window on escape
		if (inputManager.escape.isPressed())
			break;
		if (inputManager.tab.isPressed())
		{
			if (!sVar.isPannelHide)
			{
				inputManager.mouse.setVisible(context.window, false);
				inputManager.mouse.goTo(context.window, WIN_W / 2, WIN_H / 2);
				sVar.pannelVector[0].setPosToGo(WIN_W, 0.0f);
				sVar.pannelVector[1].addPosToGo(120, 0);
				sVar.pannelVector[2].addPosToGo(-120, 0);
				sVar.pannelVector[3].addPosToGo(-120, 0);
			}
			else
			{
				inputManager.mouse.setVisible(context.window, true);
				sVar.pannelVector[0].setPosToGo(WIN_W - 120, 0.0f);
				sVar.pannelVector[1].addPosToGo(-120, 0);
				sVar.pannelVector[2].addPosToGo(120, 0);
				sVar.pannelVector[3].addPosToGo(120, 0);
			}
			sVar.isPannelHide = !sVar.isPannelHide;
		}
		if (!sVar.isPannelHide)
		{
			//wave parameter
			sVar.waveHeight =  sVar.pannelVector[1][0.0f].getValue() * 2 * WAVE_HEIGHT;
			sVar.waveVelocity =  sVar.pannelVector[1][1.0f].getValue() * 2 * WAVE_VELOCITY;
			sVar.waveThickess =  sVar.pannelVector[1][2.0f].getValue() * 2 * WAVE_THICKNESS;
			//rain parameter
			sVar.rainIntensity =  sVar.pannelVector[2][0.0f].getValue() * 2 * RAIN_INTENSITY;
			sVar.rainDelay =  sVar.pannelVector[2][1.0f].getValue() * 2 * RAIN_TIME_BEFORE_NEW_PARTICULE;
			//filling parameter
			sVar.fillingIntensity =  sVar.pannelVector[3][0.0f].getValue() * 2 * FILLING_INTENSITY;
			sVar.fillingDelay =  sVar.pannelVector[3][1.0f].getValue() * 2 * FILLING_TIME_BEFORE_NEW_PARTICULE;
		}


		// Compute part
		computation(&inputManager, &camera, &context, &sVar, &simulation, &shaderManager);

		// Drawing part
		draw(context.window, &camera, &terrain,
			&shaderManager, &sVar, &simulation);
	}

	context.close();
	glfwTerminate();
	return (0);
}


static void	events(
				GLFWwindow* window,
				InputManager *inputManager)
{
	// Update events
	glfwPollEvents();

	inputManager->update(window);
}


static void	computation(
				InputManager *inputManager,
				Camera *camera,
				OpenGLContext *context,
				t_simulationVariable *sVar,
				WaterSimulation	*simulation,
				ShaderManager *shaderManager)
{
	static std::vector<double> deltas;
	static double	timePrintFps = 0.0;
	static double	timeRainningParticuleAdd = 0.0;
	static double	timeFillingParticuleAdd = 0.0;
	static double	lastTime = 0.0;
	double			currentTime, delta, cameraSpeed;

	currentTime = glfwGetTime();
	delta = currentTime - lastTime;
	lastTime = currentTime;

	timePrintFps += delta;
	deltas.push_back(delta);
	if (timePrintFps >= PRINT_FPS_TIME)
	{
		timePrintFps -= PRINT_FPS_TIME;
		double avg = 0.0;
		for (double dtime : deltas)
		{
			avg += dtime;
		}
		avg /= deltas.size();
		printf("fps : %8.3f, %5i particules\n", 1.0 / avg,
				simulation->getNbParticules());
		deltas.clear();

	}
	timeRainningParticuleAdd += delta;

	if (timeRainningParticuleAdd >= sVar->rainDelay)
	{
		timeRainningParticuleAdd -= sVar->rainDelay;
		double avg = 0.0;
		for (double dtime : deltas)
		{
			avg += dtime;
		}
		avg /= deltas.size();
		if (sVar->isRainning)
			updateRain(simulation, sVar);
	}
	timeFillingParticuleAdd += delta;

	if (timeFillingParticuleAdd >= sVar->fillingDelay)
	{
		timeFillingParticuleAdd -= sVar->fillingDelay;
		double avg = 0.0;
		for (double dtime : deltas)
		{
			avg += dtime;
		}
		avg /= deltas.size();
		if (sVar->isFilling)
			fillingPool(simulation, sVar);
	}
	for (Pannel & pannel : sVar->pannelVector)
	{
		pannel.tick(delta);
	}
	

	// To avoid big simulation step
	if (delta > MINIMUM_SIMULATION_UPDATE)
		delta = MINIMUM_SIMULATION_UPDATE;

	cameraSpeed = CAMERA_SPEED * delta;
	if (inputManager->lcontrol.isDown())
		cameraSpeed *= CAMERA_SPRINT_FACTOR;

	// Camera info
	if (inputManager->t.isPressed())
		camera->printInfo();

	// Camera translation
	if (inputManager->w.isDown() || inputManager->z.isDown())
		camera->moveFront(cameraSpeed);
	else if (inputManager->s.isDown())
		camera->moveFront(-cameraSpeed);

	if (inputManager->a.isDown() || inputManager->q.isDown())
		camera->moveRight(cameraSpeed);
	else if (inputManager->d.isDown())
		camera->moveRight(-cameraSpeed);

	if (inputManager->space.isDown())
		camera->moveUp(cameraSpeed);
	else if (inputManager->lshift.isDown())
		camera->moveUp(-cameraSpeed);

	// Camera rotation
	if (sVar->isPannelHide)
	{
		const glm::vec2		cursorMidPos(WIN_W / 2, WIN_H / 2);
		camera->rotateY((cursorMidPos.x - inputManager->mouse.getPos().x) * delta * CAMERA_ROTATION_SPEED_MOUSE * -1);
		camera->rotateX((cursorMidPos.y - inputManager->mouse.getPos().y) * delta * CAMERA_ROTATION_SPEED_MOUSE);
		inputManager->mouse.goTo((*context).window, cursorMidPos.x, cursorMidPos.y);
	}
	if (inputManager->up.isDown())
		camera->rotateX(CAMERA_ROTATION_SPEED * delta);
	else if (inputManager->down.isDown())
		camera->rotateX(-CAMERA_ROTATION_SPEED * delta);

	if (inputManager->left.isDown())
		camera->rotateY(-CAMERA_ROTATION_SPEED * delta);
	else if (inputManager->right.isDown())
		camera->rotateY(CAMERA_ROTATION_SPEED * delta);

	simulation->tick(shaderManager, delta);
}


static void	draw(
				GLFWwindow* window,
				Camera *camera,
				Terrain *terrain,
				ShaderManager *shaderManager,
				t_simulationVariable *sVar,
				WaterSimulation	*simulation)
{
	// Clear window
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh
	// waterManager->draw(camera, shaderManager);
	// glBindVertexArray(0);


	// Button	test(10, 10, 100, 50,addWater, simulation, textureManager->getTexture("rain"));
	for (Pannel & pannel : sVar->pannelVector)
	{
		pannel.renderMesh(shaderManager);
	}


	// test.renderMesh(shaderManager);
	terrain->renderMesh(camera, shaderManager);
	simulation->draw(camera, shaderManager);

	// Display the new image
	glfwSwapBuffers(window);
}


void	addWater(void * arg)
{
	WaterSimulation & simulation = *((WaterSimulation *)(arg));
	simulation.addWater(glm::vec3(5, 5, 5));
}
