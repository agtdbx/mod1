#include <model/Button.hpp>

// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
// #include <stdexcept>

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Camera Button::camera = Camera();

Button::Button(float x, float y, float width, float height)
:x_screen(x), y_screen(y), width(width), height(height)
{
	Button::camera.setPosition(glm::vec3(0,0,0));

}


Button::Button(const Button &obj)
:x_screen(obj.x_screen), y_screen(obj.y_screen), width(obj.width), height(obj.height)
{
}

//---- Destructor --------------------------------------------------------------

Button::~Button()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

Button	&Button::operator=(const Button &obj)
{
	if (this == &obj)
		return (*this);
	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Button::renderMesh( ShaderManager *shaderManager)
{
	MenuShader					*menuShader;
	std::vector<Point2D>		points;
	std::vector<float>			vertices;
	unsigned int				indices[6] = {0, 1, 2, 3, 1, 2};

	menuShader = shaderManager->getMenuShader();

	points.push_back(Point2D(Vec2(x_screen + width, y_screen), 0.5, 0.5, 0.5));
	points.push_back(Point2D(Vec2(x_screen + width, y_screen), 0.5, 0.5, 0.5));
	points.push_back(Point2D(Vec2(x_screen, y_screen + height),  0.5, 0.5, 0.5));
	points.push_back(Point2D(Vec2(x_screen + width, y_screen + height), 0.5, 0.5, 0.5));
	// indices.push_back((t_tri_id){0, 1, 2});
	// indices.push_back((t_tri_id){3, 1, 2});

	printf("\n\n\n\n");
	for (Point2D &point : points)
	{
		float x = (point.pos.x  / WIN_W) * 2 - 1;
		float y = (point.pos.y  / WIN_H) * 2 - 1;

		vertices.push_back(x);
		vertices.push_back(y);
		// vertices.push_back(point.r);
		// vertices.push_back(point.g);
		// vertices.push_back(point.b);

		printf("POINT (%.3f, %.3f) = {%f %f %f}\n", x, y, point.r, point.g, point.b);
	}
	

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,shaderManager->getEBOId());

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);
	menuShader->use();
	glBindVertexArray(shaderManager->getVAOId());
	
	// glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	// std::cout << "test" << std::endl;
	// this->mesh.draw(&Button::camera, shaderManager.getShader("terrain"), shaderManager.getVAOId());
	// this->mesh.draw(&camera, shaderManager.getShader("terrain"), shaderManager.getVAOId());
}

//**** PRIVATE METHODS *********************************************************

