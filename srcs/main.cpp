#include <define.hpp>
#include <engine/inputs/InputManager.hpp>

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


void	draw(GLFWwindow* window)
{
	// Clear window
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw triangle
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Display the new image
	glfwSwapBuffers(window);
}


bool	setup_scene(void)
{
	// Create vertice
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};


	// Allocate memory for vertice array into gpu
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Allocate memory into gpu
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// Set the new allocated buffer as array one
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Copy vertice into gpu buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex shader
	const char *vertexShaderSource =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	// Create vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Put shader code into variable
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// And compile it
	glCompileShader(vertexShader);

	// Check if shader compilation succeed
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "Vertex shader compilation failed\n" << infoLog << std::endl;
		return (false);
	}

	// Create fragment shader
	const char *fragmentShaderSource =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Check if shader compilation succeed
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "Fragment shader compilation failed\n" << infoLog << std::endl;
		return (false);
	}

	// Create shader program
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	// Link vertex and fragment shader to shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Link shader to gpu
	glLinkProgram(shaderProgram);

	// Check if link
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "Shader program linking failed\n" << infoLog << std::endl;
		return (false);
	}

	// Give info about our vertex to gpu
	// (vertice array id, nb vertice, need to normalize point, size of array, thing for weird cast)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Select vertice array
	glEnableVertexAttribArray(0);

	// Select the program shader as active one
	glUseProgram(shaderProgram);

	// Delete base shader because there are useless now
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return (true);
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
	// std::cout << point_list[0] << std::endl;
	// std::cout << point_list[1] << std::endl;
	// std::cout << point_list[2] << std::endl;


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

	// Init GLEW (OpenGL Extension Wrangler)
	if (glewInit() != GLEW_OK)
		return (quit_as_error(window, "Failed to initialize GLEW"));

	// Set input management
	InputManager	inputManager;
	glfwSetScrollCallback(window, scroll_callback);

	// Set the OpenGL viewport size
	glViewport(0, 0, WIN_W, WIN_H);

	// Setup the scene
	if (!setup_scene())
		return (quit_as_error(window, "Bad scene setup"));

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
