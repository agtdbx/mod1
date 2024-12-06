#include <define.hpp>

// Error callback function for GLFW
void	print_opengl_error(int error, const char* description)
{
	std::cerr << "Opengl error : " << description << std::endl;
}


t_key	create_key(int keycode)
{
	t_key	key;

	key.keycode = keycode;
	key.pressed = false;
	key.down = false;
	key.released = false;
	return (key);
}


t_mouse	create_mouse(void)
{
	t_mouse	mouse;

	mouse.left = create_key(GLFW_MOUSE_BUTTON_LEFT);
	mouse.middle = create_key(GLFW_MOUSE_BUTTON_MIDDLE);
	mouse.right = create_key(GLFW_MOUSE_BUTTON_RIGHT);
	mouse.x = 0.0;
	mouse.y = 0.0;
	mouse.scroll = 0.0;
	return (mouse);
}


t_input_manager	init_input_manager(void)
{
	t_input_manager	input_manager;

	input_manager.mouse = create_mouse();
	input_manager.escape = create_key(GLFW_KEY_ESCAPE);
	input_manager.e = create_key(GLFW_KEY_E);
	return (input_manager);
}


void	update_key(GLFWwindow* window, t_key *key)
{
	int status;

	status = glfwGetKey(window, key->keycode);

	if (status == GLFW_PRESS && !key->down)
	{
		key->pressed = true;
		key->down = true;
		key->released = false;
	}
	else if (status == GLFW_RELEASE && key->down)
	{
		key->pressed = false;
		key->down = false;
		key->released = true;
	}
	else
	{
		key->pressed = false;
		key->released = false;
	}
}


void	update_mouse_key(GLFWwindow* window, t_key *key)
{
	int status;

	status = glfwGetMouseButton(window, key->keycode);

	if (status == GLFW_PRESS && !key->down)
	{
		key->pressed = true;
		key->down = true;
		key->released = false;
	}
	else if (status == GLFW_RELEASE && key->down)
	{
		key->pressed = false;
		key->down = false;
		key->released = true;
	}
	else
	{
		key->pressed = false;
		key->released = false;
	}
}


void	events(GLFWwindow* window, t_input_manager *input_manager)
{
	// Update events
	glfwPollEvents();

	// Update keyboard input
	update_key(window, &input_manager->escape);
	update_key(window, &input_manager->e);

	// Update mouse input
	update_mouse_key(window, &input_manager->mouse.left);
	update_mouse_key(window, &input_manager->mouse.middle);
	update_mouse_key(window, &input_manager->mouse.right);
	glfwGetCursorPos(window, &input_manager->mouse.x, &input_manager->mouse.y);
}


void	computation(t_input_manager *input_manager)
{
	// Check e key
	if (input_manager->e.pressed)
		std::cout << "key e is pressed" << std::endl;
	if (input_manager->e.down)
		std::cout << "key e is down" << std::endl;
	if (input_manager->e.released)
		std::cout << "key e is released" << std::endl;

	// Check mouse left click
	if (input_manager->mouse.left.pressed)
		std::cout << "mouse left click on " << input_manager->mouse.x << ", " << input_manager->mouse.y << std::endl;
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
int	main(void)
{
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

	t_input_manager	input_manager = init_input_manager();

	// Set the OpenGL viewport size
	glViewport(0, 0, WIN_W, WIN_H);

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		events(window, &input_manager);

		// Close window on escape
		if (input_manager.escape.pressed)
			break;

		computation(&input_manager);

		draw(window);
	}

	// Clean up and terminate
	glfwDestroyWindow(window);
	glfwTerminate();

	return (0);
}
