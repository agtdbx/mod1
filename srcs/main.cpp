#include <define.hpp>
#include <engine/inputs/InputManager.hpp>

// Error callback function for GLFW
void	print_opengl_error(int error, const char* description)
{
	std::cerr << "Opengl error : " << description << std::endl;
}


void	events(GLFWwindow* window, InputManager *inputManager)
{
	// Update events
	glfwPollEvents();

	inputManager->update(window);
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
}


void	draw(GLFWwindow* window)
{
	// Clear window
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Display the new image
	glfwSwapBuffers(window);
}


// Initialize OpenGL, create a window, and render a color
int	main(int c, char **v)
{
	//parsing
	if (c != 2)
		throw std::invalid_argument("Error : Wrong number of argument.");

	std::vector<Vec3> point_list = parse(v[1]);
	std::cout << point_list[0] << std::endl;
	std::cout << point_list[1] << std::endl;
	std::cout << point_list[2] << std::endl;


	// Init opengl
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return (1);
	}

	// Set opengl print error function
	glfwSetErrorCallback(print_opengl_error);

	// Create window
	GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, WIN_TITLE, nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return (1);
	}

	// Setup opengl context
	glfwMakeContextCurrent(window);

	// Init GLEW (OpenGL Extension Wrangler)
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW!" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return (1);
	}

	InputManager	inputManager;

	// Set the OpenGL viewport size
	glViewport(0, 0, WIN_W, WIN_H);

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		events(window, &inputManager);

		// Close window on escape
		if (inputManager.escape.isPressed())
			break;

		computation(&inputManager);

		draw(window);
	}

	// Clean up and terminate
	glfwDestroyWindow(window);
	glfwTerminate();

	return (0);
}
