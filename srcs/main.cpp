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
				WaterSimulation	*simulation,
				ShaderManager *shaderManager,
				Terrain *terrain,
				double	deltaConst);
static void	draw(
				GLFWwindow* window,
				Camera *camera,
				ShaderManager *shaderManager,
				Terrain *terrain,
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

	OpenGLContext	context;

	if (!context.isInitGood())
	{
		glfwTerminate();
		return (1);
	}

	Terrain			terrain;
	InputManager	inputManager(context.window);
	ShaderManager	shaderManager;
	TextureManager	textureManager;
	WaterSimulation	simulation;
	Camera			camera;

	t_simulationVariable	sVar;

	Button::mouse = &inputManager.mouse;
	Slider::mouse = &inputManager.mouse;

	try
	{
		terrain.loadFromFile(argv[1]);
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
	initUi(&sVar, &textureManager, &simulation);

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
			sVar.pannelVector[5][7].setColor(sVar.watercolor, BUTTON_BASE_COLOR_TYPE);

		}

		// Compute part
		if (sVar.needStep)
		{
			computation(&inputManager, &camera, &context, &sVar, &simulation,
						&shaderManager, &terrain, 1.0f/100.0f);
		}
		else
			computation(&inputManager, &camera, &context, &sVar, &simulation,
						&shaderManager, &terrain, 0.0);
		sVar.needStep = false;

		// Drawing part
		draw(context.window, &camera, &shaderManager, &terrain, &sVar, &simulation);
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
				ShaderManager *shaderManager,
				Terrain *terrain,
				double	deltaConst)
{
	static int		nbCall = 0;
	static double	timePrintFps = 0.0;
	static double	timeRainningParticuleAdd = 0.0;
	static double	timeFillingParticuleAdd = 0.0;
	static double	lastTime = 0.0;
	double			currentTime, delta, cameraSpeed;

	currentTime = glfwGetTime();
	delta = currentTime - lastTime;
	lastTime = currentTime;

	timePrintFps += delta;
	nbCall++;
	if (timePrintFps >= PRINT_FPS_TIME)
	{
		double	avg = timePrintFps / (double)nbCall;
		printf("fps : %8.3f, %5i particules\n", 1.0 / avg,
				simulation->getNbParticules());
		timePrintFps -= PRINT_FPS_TIME;
		nbCall = 0;
	}
	if (deltaConst)
		timeRainningParticuleAdd += deltaConst;
	else
		timeRainningParticuleAdd += delta;
	if (timeRainningParticuleAdd >= sVar->rainDelay)
	{
		timeRainningParticuleAdd -= sVar->rainDelay;
		if (sVar->isRainning && (!sVar->isStopped || deltaConst))
			updateRain(simulation, sVar);
	}

	if (deltaConst)
		timeFillingParticuleAdd += deltaConst;
	else
		timeFillingParticuleAdd += delta;
	if (timeFillingParticuleAdd >= sVar->fillingDelay)
	{
		timeFillingParticuleAdd -= sVar->fillingDelay;
		if (sVar->isFilling && (!sVar->isStopped || deltaConst))
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
	if (!sVar->isStopped)
		simulation->tick(shaderManager, terrain, delta);
	if (deltaConst)
		simulation->tick(shaderManager, terrain, deltaConst);
}


static void	draw(
				GLFWwindow* window,
				Camera *camera,
				ShaderManager *shaderManager,
				Terrain *terrain,
				t_simulationVariable *sVar,
				WaterSimulation	*simulation)
{
	// Clear window
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Pannel & pannel : sVar->pannelVector)
	{
		pannel.renderMesh(shaderManager);
	}

	// test.renderMesh(shaderManager);
	terrain->renderMesh(camera, shaderManager);
	simulation->draw(camera, shaderManager, terrain, &sVar->watercolor);
	// simulation->drawTest(camera, shaderManager, terrain, &sVar->watercolor);

	// Display the new image
	glfwSwapBuffers(window);
}


void	addWater(void * arg)
{
	WaterSimulation & simulation = *((WaterSimulation *)(arg));
	simulation.addWater(glm::vec3(5, 5, 5));
}
