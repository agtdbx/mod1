#include <model/Button.hpp>

// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
// #include <stdexcept>

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Mouse *Button::mouse = NULL;

Button::Button(float x, float y, float width, float height, void (*functionToExecute)(void *), void *arg, unsigned int texture)
:x_screen(x), y_screen(y), width(width), height(height)
{
	this->texture = texture;
	this->arg = arg;
	this->active_B = true;
	this->functionToExecute = functionToExecute;
	this->baseColor = glm::vec3(0.5,0.5,0.5);
	this->underlineColor = glm::vec3(0.4,0.4,0.4);;
}

Button::Button(float x, float y, float width, float height, void (*functionToExecute)(void *), void *arg, unsigned int texture, glm::vec3 baseColor, glm::vec3 underlineColor)
:x_screen(x), y_screen(y), width(width), height(height)
{
		this->texture = texture;
	this->arg = arg;
	this->active_B = true;
	this->functionToExecute = functionToExecute;
	this->baseColor = baseColor;
	this->underlineColor = underlineColor;
}



Button::Button(const Button &obj)
:x_screen(obj.x_screen), y_screen(obj.y_screen), width(obj.width), height(obj.height)
{
		this->texture = obj.texture;
	this->arg = obj.arg;
	this->active_B = obj.active_B;
	this->functionToExecute = obj.functionToExecute;
	this->baseColor = obj.baseColor;
	this->underlineColor = obj.underlineColor;
}

//---- Destructor --------------------------------------------------------------

Button::~Button()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

bool	Button::isActive()
{
	return this->active_B;
}


//---- Setters -----------------------------------------------------------------

void	Button::active()
{
	this->active_B = true;
}

void	Button::desactive()
{
	this->active_B = false;
}

void	Button::setArg(void *arg)
{
	this->arg = arg;
}

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
	float						texturePos[4][2] = {{0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f}};

	menuShader = shaderManager->getMenuShader();

	if (Button::mouse && this->active_B && this->mouseOnButton())
	{
		points.push_back(Point2D(Vec2(x_screen, y_screen), this->underlineColor[0], this->underlineColor[1], this->underlineColor[2]));
		points.push_back(Point2D(Vec2(x_screen + width, y_screen), this->underlineColor[0], this->underlineColor[1], this->underlineColor[2]));
		points.push_back(Point2D(Vec2(x_screen, y_screen + height),  this->underlineColor[0], this->underlineColor[1], this->underlineColor[2]));
		points.push_back(Point2D(Vec2(x_screen + width, y_screen + height), this->underlineColor[0], this->underlineColor[1], this->underlineColor[2]));
		if (Button::mouse->left.isPressed())
			this->press();
	}
	else
	{
		points.push_back(Point2D(Vec2(x_screen, y_screen), this->baseColor[0], this->baseColor[1], this->baseColor[2]));
		points.push_back(Point2D(Vec2(x_screen + width, y_screen), this->baseColor[0], this->baseColor[1], this->baseColor[2]));
		points.push_back(Point2D(Vec2(x_screen, y_screen + height),  this->baseColor[0], this->baseColor[1], this->baseColor[2]));
		points.push_back(Point2D(Vec2(x_screen + width, y_screen + height), this->baseColor[0], this->baseColor[1], this->baseColor[2]));
	}

	for (int i = 0; i < 4; i++)
	{
		Point2D &point = points[i];
		float x = (point.pos.x  / WIN_W) * 2 - 1;
		float y = ((WIN_H - point.pos.y)  / WIN_H) * 2 - 1;

		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(point.r);
		vertices.push_back(point.g);
		vertices.push_back(point.b);
		vertices.push_back(texturePos[i][0]);
		vertices.push_back(texturePos[i][1]);
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);


	menuShader->use();

	glBindVertexArray(shaderManager->getVAOId());
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void	Button::press()
{
	if (this->active_B)
		this->functionToExecute(this->arg);
}



//**** PRIVATE METHODS *********************************************************

bool 	Button::mouseOnButton()
{
	if (Button::mouse->getPos().x >= this->x_screen and Button::mouse->getPos().x < this->x_screen + this->width and
		Button::mouse->getPos().y >= this->y_screen and Button::mouse->getPos().y < this->y_screen + this->height)
		return true;
	return false;
}