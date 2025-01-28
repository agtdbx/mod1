#include <fstream>
#include <sstream>

#include <ctime>
#include <define.hpp>
#include <interfaceDeclaration.hpp>
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
#include <model/TextEntry.hpp>
#include <model/WaterSimulation.hpp>

void		loadTexture(
				TextureManager *textureManager,
				ShaderManager *shaderManager);
void		initUi(
				t_simulationVariable *sVar,
				TextureManager *textureManager,
				WaterSimulation *simulation);
static void	events(
				GLFWwindow* window,
				InputManager *inputManager);
static void	computation(
				InputManager *inputManager,
				Camera *camera,
				OpenGLContext *context,
				t_simulationVariable *sVar,
				t_performanceLog *perfLog,
				WaterSimulation	*simulation,
				ShaderManager *shaderManager,
				Terrain *terrain,
				double	delta);
static void	draw(
				GLFWwindow* window,
				Camera *camera,
				ShaderManager *shaderManager,
				Terrain *terrain,
				t_simulationVariable *sVar,
				t_performanceLog *perfLog,
				WaterSimulation	*simulation);


int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Error : Wrong number of argument" << std::endl;
		return (1);
	}

	std::vector<Vec3> parseTab;
	std::vector<std::vector<double>> heightmap;

	try
	{
		parseTab = parse(argv[1]);
		heightmap = interpolate(parseTab);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
		return (1);
	}

	if (!glfwInit())
	{
		std::cerr << "Error : Opengl init failed" << std::endl;
		return (1);
	}

	OpenGLContext	context;

	if (!context.isInitGood())
	{
		glfwTerminate();
		return (1);
	}

	Terrain				terrain;
	InputManager		inputManager(context.window);
	ShaderManager		shaderManager;
	TextureManager		textureManager;
	WaterSimulation		simulation;
	Camera				camera;

	t_simulationVariable	sVar;

	Button::mouse = &inputManager.mouse;
	Slider::mouse = &inputManager.mouse;
	TextEntry::mouse = &inputManager.mouse;
	TextEntry::inputManager = &inputManager;
	TextEntry::textureManager = &textureManager;

	try
	{
		terrain.loadFromParse(parseTab, heightmap);
		loadTexture(&textureManager, &shaderManager);
		Slider::texture = textureManager.getTexture("noTexture");

	}
	catch (std::exception &e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
		context.close();
		glfwTerminate();
		return (1);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	initUi(&sVar, &textureManager, &simulation);

	// Main loop
	inputManager.mouse.goTo(context.window, WIN_W / 2, WIN_H / 2);
	inputManager.mouse.setVisible(context.window, false);
	t_performanceLog	perfLog = (t_performanceLog){0};
	double				delta;
	double				currentTime = 0;
	double				lastTime = 0;

	while (!glfwWindowShouldClose(context.window))
	{
		currentTime = glfwGetTime();
		delta = currentTime - lastTime;
		lastTime = currentTime;

		perfLog.deltas += delta;
		perfLog.timeBeforePrint -= delta;
		perfLog.nbCall++;

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
				sVar.pannelVector[6].addPosToGo(-240, 0);
				sVar.pannelVector[4].addPosToGo(0, -58);
				sVar.pannelVector[5].addPosToGo(0, 300);
			}
			else
			{
				inputManager.mouse.setVisible(context.window, true);
				sVar.pannelVector[0].setPosToGo(WIN_W - 230, 0.0f);
				sVar.pannelVector[1].addPosToGo(-120, 0);
				sVar.pannelVector[2].addPosToGo(120, 0);
				sVar.pannelVector[3].addPosToGo(120, 0);
				sVar.pannelVector[6].addPosToGo(240, 0);
				sVar.pannelVector[4].addPosToGo(0, 58);
				sVar.pannelVector[5].addPosToGo(0, -300);
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
			//settings parameter
			sVar.cameraSensibility = sVar.pannelVector[5][0.0f].getValue() * 2 *  CAMERA_ROTATION_SPEED_MOUSE;
			sVar.sprintSpeed = sVar.pannelVector[5][1.0f].getValue() * 2 *  CAMERA_SPRINT_FACTOR;
			sVar.watercolor = glm::vec3(sVar.pannelVector[5][2.0f].getValue(),
										sVar.pannelVector[5][3.0f].getValue(),
										sVar.pannelVector[5][4.0f].getValue());
			sVar.waterDensity = sVar.pannelVector[5][5.0f].getValue() * (2) *  WATER_DENSITY;
			// sVar.waterDensity = sVar.pannelVector[5][5.0f].getValue() * ( sVar.pannelVector[5][5.0f].getValue()) *  WATER_DENSITY;
			sVar.pannelVector[5][7].setColor(sVar.watercolor, BUTTON_BASE_COLOR_TYPE);
			//generate parameter
			if (sVar.pannelVector[6][(char) 0].getValue().size())
			{
				sVar.generatePos.x = std::stoi(sVar.pannelVector[6][(char) 0].getValue());
				if (sVar.generatePos.x >= MAP_SIZE)
					sVar.generatePos.x = GENERATE_MAX_X;
			}
			else
			{
				sVar.generatePos.x = GENERATE_MIN_X;
			}
			if (sVar.pannelVector[6][(char) 1].getValue().size())
			{
				sVar.generatePos.y = std::stoi(sVar.pannelVector[6][(char)1].getValue());
				if (sVar.generatePos.y > MAP_MAX_HEIGHT)
					sVar.generatePos.y = MAP_MAX_HEIGHT;
			}
			else
			{
				sVar.generatePos.y = GENERATE_PADDING;
			}
				if (sVar.pannelVector[6][(char) 2].getValue().size())
			{
				sVar.generatePos.z = std::stoi(sVar.pannelVector[6][(char) 2].getValue());
				if (sVar.generatePos.z >= MAP_SIZE)
					sVar.generatePos.z = GENERATE_MAX_Z;
			}
			else
			{
				sVar.generatePos.z = GENERATE_MIN_Z;
			}
			sVar.generateIntensity =  sVar.pannelVector[6][0.0f].getValue() * 2 * GENERATE_INTENSITY;
			sVar.generateDelay =  sVar.pannelVector[6][1.0f].getValue() * 2 * GENERATE_TIME_BEFORE_NEW_PARTICULE;
		}

		// Compute part
		computation(&inputManager, &camera, &context, &sVar, &perfLog,
					&simulation, &shaderManager, &terrain, delta);

		// Drawing part
		draw(context.window, &camera, &shaderManager, &terrain,
				&sVar, &perfLog, &simulation);

		if (perfLog.timeBeforePrint <= 0.0)
		{
			// Compute main stats
			float	avgDelta = perfLog.deltas / perfLog.nbCall;
			printf("fps : %8.3f, %5i particules\n",
					1.0 / avgDelta, simulation.getNbParticules());

			if (perfLog.moreStats)
			{
				printf("average delta %.6f\n", avgDelta);

				// Compute computation stats
				printf("Computation stats : \n");
				float	predictedPos = perfLog.timePredictedPos / perfLog.nbCall;
				float	putInGrid = perfLog.timePutInGrid / perfLog.nbCall;
				float	density = perfLog.timeComputeDensity / perfLog.nbCall;
				float	mapDensity = perfLog.timeComputeMapDensity / perfLog.nbCall;
				float	pressure = perfLog.timeApplyPressure / perfLog.nbCall;
				float	updatePos = perfLog.timeUpdatePositions / perfLog.nbCall;
				float	computeTotal = predictedPos + putInGrid + density
										+ mapDensity + pressure + updatePos;
				printf(" - predicted pos %9.3f ms\n", predictedPos * 1000.0f);
				printf(" - put in grid   %9.3f ms\n", putInGrid * 1000.0f);
				printf(" - density       %9.3f ms\n", density * 1000.0f);
				printf(" - map density   %9.3f ms\n", mapDensity * 1000.0f);
				printf(" - pressure      %9.3f ms\n", pressure * 1000.0f);
				printf(" - update pos    %9.3f ms\n", updatePos * 1000.0f);
				printf(" - total compute %9.3f ms\n", computeTotal * 1000.0f);

				// Compute draw stats
				printf("Draw stats : \n");
				float	drawTerrain = perfLog.timeDrawTerrain / perfLog.nbCall;
				float	drawWater = perfLog.timeDrawWater / perfLog.nbCall;
				float	swapBuffer = perfLog.timeSwapBuffer / perfLog.nbCall;
				float	drawTotal = drawTerrain + drawWater + swapBuffer;
				printf(" - draw terrain  %9.3f ms\n", drawTerrain * 1000.0f);
				printf(" - draw water    %9.3f ms\n", drawWater * 1000.0f);
				printf(" - glSwapBuffer  %9.3f ms\n", swapBuffer * 1000.0f);
				printf(" - total draw    %9.3f ms\n", drawTotal * 1000.0f);

				// Total stats
				float	totalTime = computeTotal + drawTotal;
				printf("Total time per tick %9.3f ms\n\n", totalTime * 1000.0f);
			}

			// Reset main times
			perfLog.deltas = 0.0;
			perfLog.timeBeforePrint += PRINT_FPS_TIME;
			perfLog.nbCall = 0;
			// Reset computation times
			perfLog.timePredictedPos = 0.0;
			perfLog.timePutInGrid = 0.0;
			perfLog.timeComputeDensity = 0.0;
			perfLog.timeComputeMapDensity = 0.0;
			perfLog.timeApplyPressure = 0.0;
			perfLog.timeUpdatePositions = 0.0;
			// Reset draw times
			perfLog.timeDrawTerrain = 0.0;
			perfLog.timeDrawWater = 0.0;
			perfLog.timeSwapBuffer = 0.0;
		}
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
				t_performanceLog *perfLog,
				WaterSimulation	*simulation,
				ShaderManager *shaderManager,
				Terrain *terrain,
				double delta)
{
	static double	timeRainningParticuleAdd = 0.0;
	static double	timeFillingParticuleAdd = 0.0;
	static double	timeGenerateParticuleAdd = 0.0;
	double			cameraSpeed;

	for (Pannel & pannel : sVar->pannelVector)
	{
		pannel.tick(delta);
	}

	// To avoid big simulation step
	if (delta > MINIMUM_SIMULATION_UPDATE)
		delta = MINIMUM_SIMULATION_UPDATE;

	// Enable or disable more log when press tab
	if (inputManager->alt.isPressed())
		perfLog->moreStats = !perfLog->moreStats;

	// Compute camera speed
	cameraSpeed = CAMERA_SPEED * delta;
	if (inputManager->lcontrol.isDown())
		cameraSpeed *= sVar->sprintSpeed;

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
		camera->rotateY((cursorMidPos.x - inputManager->mouse.getPos().x) * sVar->cameraSensibility * -1);
		camera->rotateX((cursorMidPos.y - inputManager->mouse.getPos().y) * sVar->cameraSensibility);
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

	// Water simulation
	if (sVar->isStopped || sVar->needStep)
		return ;

	if (sVar->isStopped && sVar->needStep)
		delta = 0.01;

	timeRainningParticuleAdd += delta;
	if (timeRainningParticuleAdd >= sVar->rainDelay)
	{
		timeRainningParticuleAdd -= sVar->rainDelay;
		if (sVar->isRainning && (!sVar->isStopped || sVar->needStep))
			updateRain(simulation, sVar);
	}

	timeFillingParticuleAdd += delta;
	if (timeFillingParticuleAdd >= sVar->fillingDelay)
	{
		timeFillingParticuleAdd -= sVar->fillingDelay;
		if (sVar->isFilling && (!sVar->isStopped || sVar->needStep))
			fillingPool(simulation, sVar);
	}
	timeGenerateParticuleAdd += delta;
	if (timeGenerateParticuleAdd >= sVar->generateDelay)
	{
		timeGenerateParticuleAdd -= sVar->generateDelay;
		if (sVar->isGenerate && (!sVar->isStopped || sVar->needStep))
			generateAt(simulation, sVar);
	}

	if (!sVar->isStopped || sVar->needStep)
		simulation->tick(shaderManager, terrain, perfLog, delta);
}


static void	draw(
				GLFWwindow* window,
				Camera *camera,
				ShaderManager *shaderManager,
				Terrain *terrain,
				t_simulationVariable *sVar,
				t_performanceLog *perfLog,
				WaterSimulation	*simulation)
{
	std::clock_t	start;

	// Clear window
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Pannel & pannel : sVar->pannelVector)
	{
		pannel.renderMesh(shaderManager);
	}

	if (perfLog->moreStats)
	{
		start = std::clock();
		terrain->renderMesh(camera, shaderManager);
		perfLog->timeDrawTerrain += (double)(std::clock() - start) / CLOCKS_PER_SEC;

		start = std::clock();
		simulation->draw(camera, shaderManager, terrain, &sVar->watercolor, sVar->waterDensity);
		perfLog->timeDrawWater += (double)(std::clock() - start) / CLOCKS_PER_SEC;

		start = std::clock();
		glfwSwapBuffers(window);
		perfLog->timeSwapBuffer += (double)(std::clock() - start) / CLOCKS_PER_SEC;
	}
	else
	{
		terrain->renderMesh(camera, shaderManager);
		simulation->draw(camera, shaderManager, terrain, &sVar->watercolor, sVar->waterDensity);
		glfwSwapBuffers(window);
	}
}
