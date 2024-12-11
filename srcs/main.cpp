#include <fstream>
#include <sstream>

#include <define.hpp>
#include <engine/inputs/InputManager.hpp>
#include <engine/render/Shader.hpp>
#include <engine/render/Mesh.hpp>
#include <engine/render/TextureManager.hpp>
#include <engine/render/Camera.hpp>
#include <engine/OpenGLContext.hpp>
#include <model/Terrain.hpp>

void	events(GLFWwindow* window, InputManager *inputManager);
void	computation(InputManager *inputManager, Camera *camera);
void	draw(GLFWwindow* window, Camera *camera,
				Terrain *terrain, Shader *shader,
				TextureManager *textureManager);


int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Error : Wrong number of argument" << std::endl;
		return (1);
	}

	OpenGLContext	context;
	InputManager	inputManager(context.window);
	Terrain			terrain;
	Shader			shader;
	TextureManager	textureManager;
	Camera			camera;
	try
	{
		terrain.loadFromFile(argv[1]);
		textureManager.addTexture("dirt", "data/textures/dirt.png");
		shader.load("data/shaders/perceptive.vs",
					"data/shaders/colors.fs");
		// shader.load("data/shaders/perceptive-texture.vs",
		// 			"data/shaders/perceptive-texture.fs");
	}
	catch (std::exception &e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
		return (1);
	}

	// Main loop
	while (!glfwWindowShouldClose(context.window)) {
		events(context.window, &inputManager);

		// Close window on escape
		if (inputManager.escape.isPressed())
			break;

		computation(&inputManager, &camera);

		draw(context.window, &camera, &terrain, &shader, &textureManager);
	}

	return (0);
}


void	events(GLFWwindow* window, InputManager *inputManager)
{
	// Update events
	glfwPollEvents();

	inputManager->update(window);
}


void	computation(InputManager *inputManager, Camera *camera)
{
	static double	lastTime = 0.0;
	double			currentTime, delta, cameraSpeed;

	currentTime = glfwGetTime();
	delta = currentTime - lastTime;
	lastTime = currentTime;

	cameraSpeed = CAMERA_SPEED * delta;
	if (inputManager->lcontrol.isDown())
		cameraSpeed *= CAMERA_SPRINT_FACTOR;

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
}


void	draw(GLFWwindow* window, Camera *camera,
			Terrain *terrain, Shader *shader,
			TextureManager *textureManager)
{
	// Clear window
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh
	terrain->renderMesh(camera, shader, textureManager, "dirt");

	// Display the new image
	glfwSwapBuffers(window);
}
