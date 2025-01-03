#include <fstream>
#include <sstream>

#include <define.hpp>
#include <engine/inputs/InputManager.hpp>
#include <engine/render/ShaderManager.hpp>
#include <engine/render/Mesh.hpp>
#include <engine/render/TextureManager.hpp>
#include <engine/render/Camera.hpp>
#include <engine/OpenGLContext.hpp>
#include <model/Terrain.hpp>
#include <model/WaterSimulation.hpp>

static void	events(
				GLFWwindow* window,
				InputManager *inputManager);
static void	computation(
				InputManager *inputManager,
				Camera *camera,
				WaterSimulation	*simulation);
static void	draw(
				GLFWwindow* window,
				Camera *camera,
				Terrain *terrain,
				ShaderManager *shaderManager,
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

	InputManager	inputManager(context.window);
	Terrain			terrain;
	ShaderManager	shaderManager;
	TextureManager	textureManager;
	WaterSimulation	simulation;
	Camera			camera;

	try
	{
		terrain.loadFromFile(argv[1]);
		textureManager.addTexture("dirt", "data/textures/dirt.png");
		shaderManager.addShader("terrain", "data/shaders/terrain.vs", "data/shaders/terrain.fs");
		shaderManager.loadWaterShaderFiles("data/shaders/water.vs", "data/shaders/water.fs");
	}
	catch (std::exception &e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
		context.close();
		glfwTerminate();
		return (1);
	}
	// simulation.addWater(glm::vec3(5, 5, 5));
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
			simulation.addWater(glm::vec3(i + 0.5, 5, j + 0.5));
	}

	// Main loop
	while (!glfwWindowShouldClose(context.window))
	{
		events(context.window, &inputManager);

		// Close window on escape
		if (inputManager.escape.isPressed())
			break;

		// Compute part
		computation(&inputManager, &camera, &simulation);

		// Drawing part
		draw(context.window, &camera, &terrain,
			&shaderManager, &simulation);
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
				WaterSimulation	*simulation)
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

	simulation->tick(delta);
}


static void	draw(
				GLFWwindow* window,
				Camera *camera,
				Terrain *terrain,
				ShaderManager *shaderManager,
				WaterSimulation	*simulation)
{
	// Clear window
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh
	// waterManager->draw(camera, shaderManager);
	terrain->renderMesh(camera, shaderManager);
	simulation->draw(camera, shaderManager);

	// Display the new image
	glfwSwapBuffers(window);
}
