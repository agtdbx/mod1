#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Error callback function for GLFW
void	glfw_error_callback(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << std::endl;
}

// Initialize OpenGL, create a window, and render a color
int	main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return -1;
	}

	// Set GLFW error callback
	glfwSetErrorCallback(glfw_error_callback);

	// Create a GLFW windowed mode window and OpenGL context
	GLFWwindow* window = glfwCreateWindow(800, 600, "Simple OpenGL Window", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Make the OpenGL context current
	glfwMakeContextCurrent(window);

	// Initialize GLEW (OpenGL Extension Wrangler)
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW!" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	// Set the OpenGL viewport size
	glViewport(0, 0, 800, 600);

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		// Process input
		glfwPollEvents();

		// Clear the color buffer (with a light blue color)
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Swap buffers (display the rendered image)
		glfwSwapBuffers(window);
	}

	// Clean up and terminate
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
