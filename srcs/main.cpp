#include <fstream>
#include <sstream>

#include <define.hpp>
#include <engine/inputs/InputManager.hpp>
#include <engine/render/Shader.hpp>
#include <engine/render/Mesh.hpp>
#include <engine/render/TextureManager.hpp>


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


void	computation(InputManager *inputManager)
{
	// Check e key
	if (inputManager->e.isPressed())
		std::cout << "key e is pressed" << std::endl;
	if (inputManager->e.isDown())
		std::cout << "key e is down" << std::endl;
	if (inputManager->e.isReleased())
		std::cout << "key e is released" << std::endl;

	// Check mouse left click
	if (inputManager->mouse.left.isPressed())
		std::cout << "mouse left click on " << inputManager->mouse.getPos() << std::endl;

	if (inputManager->mouse.getScroll() < 0.0)
		std::cout << "mouse scroll down" << std::endl;
	if (inputManager->mouse.getScroll() > 0.0)
		std::cout << "mouse scroll up" << std::endl;
}


void	draw(GLFWwindow* window, Shader *shader, Mesh *mesh, TextureManager *textureManager)
{
	// Clear window
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw mesh
	// mesh->draw(shader);
	mesh->drawWithTexture(shader, textureManager, "dirt");

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
	interpolate(point_list);

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
		// shader = Shader("data/shaders/basic.vs",
		// 				"data/shaders/red.fs");
		// shader = Shader("data/shaders/colors.vs",
		// 				"data/shaders/colors.fs");
		shader = Shader("data/shaders/texture.vs",
						"data/shaders/texture.fs");
	}
	catch (std::exception &e)
	{
		return (quit_as_error(window, e.what()));
	}

	Mesh	mesh = createMesh();
	TextureManager	textureManager;

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

		computation(&inputManager);

		draw(window, &shader, &mesh, &textureManager);
	}

	// Clean up and terminate
	glfwDestroyWindow(window);
	glfwTerminate();

	return (0);
}
