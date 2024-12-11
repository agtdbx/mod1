#include <fstream>
#include <sstream>

#include <define.hpp>
#include <engine/inputs/InputManager.hpp>
#include <engine/render/Shader.hpp>
#include <engine/render/Mesh.hpp>
#include <engine/render/TextureManager.hpp>
# include <engine/render/Camera.hpp>

double	scroll = 0.0;

// Error callback function for GLFW
void	print_opengl_error(int error, const char* description)
{
	std::cerr << "Opengl error : " << description << std::endl;
}


int	quit_as_error(GLFWwindow* window, std::string msg)
{
	std::cerr << "Error : " << msg << std::endl;

	if (window)
		glfwDestroyWindow(window);
	glfwTerminate();

	return (0);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)window;
	(void)xoffset;
	scroll = yoffset;
}


void	events(GLFWwindow* window, InputManager *inputManager)
{
	// Update events
	glfwPollEvents();

	inputManager->update(window);
	inputManager->mouse.setScroll(scroll);
	scroll = 0.0;
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

	// // Camera rotation
	if (inputManager->up.isDown())
		camera->rotateX(delta * 21.0f);
	if (inputManager->down.isDown())
		camera->rotateX(-delta * 21.0f);
	if (inputManager->left.isDown())
		camera->rotateY(-delta * 21.0f);
	if (inputManager->right.isDown())
		camera->rotateY(delta * 21.0f);

	// // Camera scaling
	// if (inputManager->mouse.getScroll() > 0.0)
	// 	mesh->scale(1.0f + delta * 10.0f);
	// if (inputManager->mouse.getScroll() < 0.0)
	// 	mesh->scale(1.0f - delta * 10.0f);

	// Check mouse left click
	if (inputManager->mouse.left.isPressed())
		std::cout << "mouse left click on " << inputManager->mouse.getPos() << std::endl;

}


void	draw(GLFWwindow* window, Camera *camera, Mesh *mesh, Shader *shader, TextureManager *textureManager)
{
	// Clear window
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw mesh
	mesh->draw(camera, shader, textureManager, "dirt");

	// Display the new image
	glfwSwapBuffers(window);
}


Mesh	createMesh(void)
{
	std::vector<Point>	vertices;
	// vertices.push_back(Point(0.5f,  0.5f, 0.0f));  // top right
	// vertices.push_back(Point(0.5f, -0.5f, 0.0f));  // bottom right
	// vertices.push_back(Point(-0.5f, -0.5f, 0.0f)); // bottom left
	// vertices.push_back(Point(-0.5f,  0.5f, 0.0f)); // top left
	vertices.push_back(Point(0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0));  // bottom right
	vertices.push_back(Point(-0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0));  // bottom left
	vertices.push_back(Point(0.0,  0.5, 0.0, 0.0, 0.0, 1.0, 0.5, 1.0));  // top

	std::vector<t_tri_id>	indices;
	// indices.push_back((t_tri_id){0, 1, 3}); // first triangle
	// indices.push_back((t_tri_id){1, 2, 3}); // second triangle
	indices.push_back((t_tri_id){0, 1, 2}); // triangle

	Mesh	mesh(vertices, indices);

	return (mesh);
}

Mesh	createMeshTest(std::vector<std::vector<double> > heighmap)
{
	std::vector<Point>	vertices;

	for (double y = 0; y < MAP_SIZE; y++)
	{
		for (double x = 0; x < MAP_SIZE; x++)
		{
			double red = double(x/ MAP_SIZE);
			double green = double(y / MAP_SIZE);
			double blue = 0;
			// double red = 0.5;
			// double green = 0.5;
			// double blue = 0.5;
			if (int(y) % 2 == 0)
			{
				if (int(x) % 2 == 0)
				{
					vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 0.0, 1.0));
				}
				else
				{
					vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 1.0, 1.0));
				}

			}
			else
			{
				if (int(x) % 2 == 0)
				{
					vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue,  1.0, 0.0));
				}
				else
				{
					vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 0.0, 0.0));
				}
			}
			// vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), double(x/ MAP_SIZE), double(y / MAP_SIZE), 0.0, 1.0, 0.0));
			// std::cout << " New point : " << Vec3((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10)) << std::endl;
		}
	}
	for (double y = 0; y < MAP_SIZE; y++)
	{
		for (double x = 0; x < MAP_SIZE; x++)
		{
			double red = double(x/ MAP_SIZE);
			double green = double(y / MAP_SIZE);
			double blue = 0;
			// double red = 0.5;
			// double green = 0.5;
			// double blue = 0.5;
			if (int(y) % 2 == 0)
			{
				if (int(x) % 2 == 0)
				{
					vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 0.0, 0.0));
				}
				else
				{
					vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 1.0, 0.0));
				}

			}
			else
			{
				if (int(x) % 2 == 0)
				{
					vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 1.0, 1.0));
				}
				else
				{
					vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 0.0, 1.0));
				}
			}
			// vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), double(x/ MAP_SIZE), double(y / MAP_SIZE), 0.0, 1.0, 0.0));
			// std::cout << " New point : " << Vec3((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (heighmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10)) << std::endl;
		}
	}

	std::vector<t_tri_id>	indices;
	for (unsigned y = 0; y < MAP_SIZE - 1; y++)
	{
		for (unsigned x = 0; x < MAP_SIZE - 1; x++)
		{
			if (y % 2 == 0)
			{
			indices.push_back((t_tri_id){y * MAP_SIZE + x, y * MAP_SIZE + x + 1, (y + 1) * MAP_SIZE + x});
			indices.push_back((t_tri_id){y * MAP_SIZE + x + 1, (y + 1) * MAP_SIZE + x, (y + 1) * MAP_SIZE + x + 1});
			}
			else
			{
			indices.push_back((t_tri_id){(MAP_SIZE * MAP_SIZE) + y * MAP_SIZE + x,(MAP_SIZE * MAP_SIZE) + y * MAP_SIZE + x + 1,(MAP_SIZE * MAP_SIZE) + (y + 1) * MAP_SIZE + x});
			indices.push_back((t_tri_id){(MAP_SIZE * MAP_SIZE) +y * MAP_SIZE + x + 1,(MAP_SIZE * MAP_SIZE) + (y + 1) * MAP_SIZE + x,(MAP_SIZE * MAP_SIZE) + (y + 1) * MAP_SIZE + x + 1});
			}
			// indices.push_back((t_tri_id){y * MAP_SIZE + x, y * MAP_SIZE + x + 1, (y + 1) * MAP_SIZE + x});
			// indices.push_back((t_tri_id){y * MAP_SIZE + x + 1, (y + 1) * MAP_SIZE + x, (y + 1) * MAP_SIZE + x + 1});
			// std::cout << " create triangle with point " << y * MAP_SIZE + x << ", "  <<  y * MAP_SIZE + x + 1 << ", "  << (y + 1) * MAP_SIZE + x
			// << " and  " << y * MAP_SIZE + x + 1 << ", "  << (y + 1) * MAP_SIZE + x << ", "  << (y + 1) * MAP_SIZE + x + 1 << std::endl;
		}
	}

	Mesh	mesh(vertices, indices);

	return (mesh);
}


// Initialize OpenGL, create a window, and render a color
int	main(int c, char **v)
{
	//parsing
	if (c != 2)
		return (quit_as_error(NULL, "Wrong number of argument"));

	std::vector<Vec3> point_list;
	try
	{
		point_list = parse(v[1]);
	}
	catch (std::exception &e)
	{
		return (quit_as_error(NULL, e.what()));
	}
	std::vector<std::vector<double>> heightmap = interpolate(point_list);

	// Init opengl
	if (!glfwInit())
		return (quit_as_error(NULL, "Failed to initialize GLFW"));

	// Set opengl print error function
	glfwSetErrorCallback(print_opengl_error);

	// Create window
	GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, WIN_TITLE, nullptr, nullptr);
	if (!window)
		return (quit_as_error(NULL, "Failed to create window"));

	// Setup opengl context
	glfwMakeContextCurrent(window);

	// To change how opengl render triangle (fill / line)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Init GLEW (OpenGL Extension Wrangler)
	if (glewInit() != GLEW_OK)
		return (quit_as_error(window, "Failed to initialize GLEW"));

	// Set input management
	InputManager	inputManager;
	glfwSetScrollCallback(window, scroll_callback);

	// Set the OpenGL viewport size
	glViewport(0, 0, WIN_W, WIN_H);

	Shader	shader;
	try
	{
		// shader.load("data/shaders/basic.vs",
		// 			"data/shaders/red.fs");
		// shader.load("data/shaders/colors.vs",
		// 			"data/shaders/colors.fs");
		// shader.load("data/shaders/texture.vs",
		// 			"data/shaders/texture.fs");
		// shader.load("data/shaders/perceptive.vs",
		// 			"data/shaders/colors.fs");
		shader.load("data/shaders/perceptive-texture.vs",
					"data/shaders/perceptive-texture.fs");
	}
	catch (std::exception &e)
	{
		return (quit_as_error(window, e.what()));
	}

	// Mesh	mesh = createMesh();
	Mesh	mesh = createMeshTest(heightmap);
	TextureManager	textureManager;

	Camera	camera;

	try
	{
		textureManager.addTexture("dirt", "data/textures/dirt.png");
	}
	catch (std::exception &e)
	{
		return (quit_as_error(window, e.what()));
	}

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		events(window, &inputManager);

		// Close window on escape
		if (inputManager.escape.isPressed())
			break;

		computation(&inputManager, &camera);

		draw(window, &camera, &mesh, &shader, &textureManager);
	}

	// Clean up and terminate
	glfwDestroyWindow(window);
	glfwTerminate();

	return (0);
}
