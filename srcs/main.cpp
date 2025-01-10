#include <fstream>
#include <sstream>

#include <define.hpp>
#include <engine/inputs/InputManager.hpp>
#include <engine/render/shader/ShaderManager.hpp>
#include <engine/render/Mesh.hpp>
#include <engine/render/TextureManager.hpp>
#include <engine/render/Camera.hpp>
#include <engine/OpenGLContext.hpp>
#include <model/Terrain.hpp>
#include <model/Button.hpp>
#include <model/WaterSimulation.hpp>

static void	events(
				GLFWwindow* window,
				InputManager *inputManager);
static void	computation(
				InputManager *inputManager,
				Camera *camera,
				WaterSimulation	*simulation,
				ShaderManager *shaderManager);
static void	draw(
				GLFWwindow* window,
				Camera *camera,
				Terrain *terrain,
				ShaderManager *shaderManager,
				std::vector<Button>	*buttonVector,
				WaterSimulation	*simulation);
void	addWater(void * arg);


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

	InputManager	inputManager(context.window);
	Terrain			terrain;
	ShaderManager	shaderManager;
	TextureManager	textureManager;
	WaterSimulation	simulation;
	Camera			camera;

	std::vector<Button>	buttonVector;
	bool				isRainning = false;
	bool				isFilling = false;

	

	Button::mouse = &inputManager.mouse;

	try
	{
		terrain.loadFromFile(argv[1]);
		textureManager.addTexture("dirt", "data/textures/dirt.png");
		textureManager.addTexture("rain", "data/textures/rainButton.png");
		textureManager.addTexture("reset", "data/textures/resetButton.png");
		textureManager.addTexture("filling", "data/textures/fillingButton.png");
		textureManager.addTexture("wave", "data/textures/waveButton.png");
		
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

	buttonVector.push_back(Button(WIN_W - 110, 10, 100, 50,addWater, &simulation, textureManager.getTexture("rain")));
	buttonVector.push_back(Button(WIN_W - 110, 70, 100, 50,addWater, &simulation, textureManager.getTexture("filling")));
	buttonVector.push_back(Button(WIN_W - 110, 130, 100, 50,addWater, &simulation, textureManager.getTexture("wave")));
	buttonVector.push_back(Button(WIN_W - 110, 190, 100, 50,addWater, &simulation, textureManager.getTexture("reset")));




	// simulation.addWater(glm::vec3(5, 5, 5));
	int	nbWater[] = {32, 32, 32};
	glm::vec3	offset(MAP_SIZE / 2 - nbWater[0] / 2, 5, MAP_SIZE / 2 - nbWater[2] / 2);
	for (int i = 0; i < nbWater[0]; i++)
	{
		for (int j = 0; j < nbWater[2]; j++)
		{
			for (int k = 0; k < nbWater[1]; k++)
			{
				simulation.addWater(glm::vec3(i, k, j) + offset);
			}
		}
	}

	// Main loop
	while (!glfwWindowShouldClose(context.window))
	{
		events(context.window, &inputManager);

		// Close window on escape
		if (inputManager.escape.isPressed())
			break;

		// if (inputManager.mouse.)
		// std::cout << inputManager.mouse.getPos() << std::endl;
		// Compute part
		computation(&inputManager, &camera, &simulation, &shaderManager);

		// Drawing part
		draw(context.window, &camera, &terrain,
			&shaderManager, &buttonVector, &simulation);
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
				WaterSimulation	*simulation,
				ShaderManager *shaderManager)
{
	static std::vector<double> deltas;
	static double	timePrintFps = 0.0;
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
				std::vector<Button>	*buttonVector,
				WaterSimulation	*simulation)
{
	// Clear window
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh
	// waterManager->draw(camera, shaderManager);
	// glBindVertexArray(0);


	// Button	test(10, 10, 100, 50,addWater, simulation, textureManager->getTexture("rain"));
	for (Button & button : *buttonVector)
	{
		button.renderMesh(shaderManager);
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