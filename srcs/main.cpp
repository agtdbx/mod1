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

	// TODO: REMOVE
	// int	max_x_gpu_worker, max_y_gpu_worker, max_z_gpu_worker, max_gpu_group;
	// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_x_gpu_worker);
	// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_y_gpu_worker);
	// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_z_gpu_worker);
	// glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &max_gpu_group);
	// printf("max_x_gpu_worker : %i\n", max_x_gpu_worker);
	// printf("max_y_gpu_worker : %i\n", max_y_gpu_worker);
	// printf("max_z_gpu_worker : %i\n", max_z_gpu_worker);
	// printf("max_gpu_group : %i\n", max_gpu_group);

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
		shaderManager.addShader("terrain", "data/shaders/terrain.glslv", "data/shaders/terrain.glslf");
		shaderManager.loadWaterShaderFiles("data/shaders/water.glslv", "data/shaders/water.glslf");
		shaderManager.addComputeShader("test", "data/shaders/test.glslc"); // TODO: REMOVE
		shaderManager.addComputeShader("test3", "data/shaders/test3.glslc"); // TODO: REMOVE
		shaderManager.addComputeShader("predictedPositions", "data/shaders/predictedPositions.glslc");
		shaderManager.addComputeShader("densities", "data/shaders/densities.glslc");
	}
	catch (std::exception &e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
		context.close();
		glfwTerminate();
		return (1);
	}
	// simulation.addWater(glm::vec3(5, 5, 5));
	int	nbWater[] = {10, 10, 10};
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

	ComputeShader	*computeTestShader = shaderManager.getComputeShader("test");

	if (computeTestShader)
	{
		printf("compute shader get\n");

		// Create data
		std::vector<float>	data;
		const int			dataSize = 10;

		for (int i = 0; i < dataSize; i++)
			data.push_back(0.0f);

		// Create texture
		unsigned int textureBuffer, texture;

		glGenBuffers(1, &textureBuffer);
		glGenTextures(1, &texture);

		computeTestShader->use();

		// Fill texture from data
		glBindBuffer(GL_TEXTURE_BUFFER, textureBuffer);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * dataSize,
						data.data(), GL_DYNAMIC_DRAW);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, texture);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, textureBuffer);

		glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

		printf("compute shader use\n");
		printf("compute shader start compute\n");

		int numGroups = (dataSize + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;
		glDispatchCompute((unsigned int)numGroups, 1, 1);

		// make sure writing to image has finished before read
		printf("compute shader wait end compute\n");
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		printf("compute shader end compute\n");

		glBindBuffer(GL_TEXTURE_BUFFER, textureBuffer);
		// Get data from texture
		glGetBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(float) * dataSize, data.data());

		printf("[");
		for (int i = 0; i < dataSize; i++)
		{
			if (i != 0)
				printf(", ");
			printf("%i", (int)data[i]);
		}
		printf("]\n");
	}

	// computeTestShader = shaderManager.getComputeShader("test3");
	// if (computeTestShader)
	// {
	// 	printf("compute shader get\n");

	// 	// Create data
	// 	std::vector<glm::vec4>	data;
	// 	const int				dataSize = 10;

	// 	for (int i = 0; i < dataSize; i++)
	// 		data.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// 	// Create texture
	// 	unsigned int textureBuffer, texture;

	// 	glGenBuffers(1, &textureBuffer);
	// 	glGenTextures(1, &texture);

	// 	computeTestShader->use();

	// 	// Fill texture from data
	// 	glBindBuffer(GL_TEXTURE_BUFFER, textureBuffer);
	// 	glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4) * dataSize,
	// 					data.data(), GL_DYNAMIC_DRAW);
	// 	glActiveTexture(GL_TEXTURE0);
	// 	glBindTexture(GL_TEXTURE_BUFFER, texture);
	// 	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, textureBuffer);

	// 	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// 	printf("compute shader use\n");
	// 	printf("compute shader start compute\n");

	// 	int numGroups = (dataSize + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;
	// 	glDispatchCompute((unsigned int)numGroups, 1, 1);

	// 	// make sure writing to image has finished before read
	// 	printf("compute shader wait end compute\n");
	// 	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	// 	printf("compute shader end compute\n");

	// 	glBindBuffer(GL_TEXTURE_BUFFER, textureBuffer);
	// 	// Get data from texture
	// 	glGetBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(glm::vec4) * dataSize, data.data());

	// 	printf("[");
	// 	for (int i = 0; i < dataSize; i++)
	// 	{
	// 		if (i != 0)
	// 			printf(", ");
	// 		printf("(%i %i %i)", (int)data[i].x, (int)data[i].y, (int)data[i].z);
	// 	}
	// 	printf("]\n");
	// }

	// Main loop
	while (!glfwWindowShouldClose(context.window))
	{
		events(context.window, &inputManager);

		// Close window on escape
		if (inputManager.escape.isPressed())
			break;

		// Compute part
		computation(&inputManager, &camera, &simulation, &shaderManager);

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
