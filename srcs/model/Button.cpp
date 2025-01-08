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
	std::vector<Point>		vertices;
	std::vector<t_tri_id>	indices;

	vertices.push_back(Point(Vec3(x_screen, y_screen, 4), Vec3(5, 5, 5), 0.5, 0.5, 0.5));
	vertices.push_back(Point(Vec3(x_screen + width, y_screen, 4), Vec3(5, 5, 5), 0.5, 0.5, 0.5));
	vertices.push_back(Point(Vec3(x_screen, y_screen + height, 4), Vec3(5, 5, 5), 0.5, 0.5, 0.5));
	vertices.push_back(Point(Vec3(x_screen + width, y_screen + height, 4), Vec3(5, 5, 5), 0.5, 0.5, 0.5));
	indices.push_back((t_tri_id){0, 1, 2});
	indices.push_back((t_tri_id){3, 1, 2});
	this->mesh = Mesh(vertices, indices);
}


Button::Button(const Button &obj)
:x_screen(obj.x_screen), y_screen(obj.y_screen), width(obj.width), height(obj.height)
{
	this->mesh = obj.mesh;
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

void	Button::renderMesh(Camera &camera, ShaderManager &shaderManager)
{
	// std::cout << "test" << std::endl;
	// this->mesh.draw(&Button::camera, shaderManager.getShader("terrain"), shaderManager.getVAOId());
	this->mesh.draw(&camera, shaderManager.getShader("terrain"), shaderManager.getVAOId());
}

//**** PRIVATE METHODS *********************************************************

