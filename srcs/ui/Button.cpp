/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Button.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gugus <gugus@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 21:51:12 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/29 19:47:55 by gugus            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ui/Button.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Mouse *Button::mouse = NULL;

Button::Button(float x, float y, float width, float height, void (*functionToExecute)(void *), void *arg, unsigned int texture)
:x_screen(x), y_screen(y), width(width), height(height)
{
	this->isON = false;
	this->isSwitch = false;
	this->texture = texture;
	this->arg = arg;
	this->active_B = true;
	this->functionToExecute = functionToExecute;
	this->baseColor = DEFAULT_BUTTON_BASE_COLOR;
	this->underlineColor = DEFAULT_BUTTON_UNDERLINE_COLOR;
}


Button::Button(float x, float y, float width, float height, void (*functionToExecute)(void *), void *arg, unsigned int texture, glm::vec3 baseColor, glm::vec3 underlineColor)
:x_screen(x), y_screen(y), width(width), height(height)
{
	this->isON = false;
	this->isSwitch = false;
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
	this->isON = obj.isON;
	this->isSwitch = obj.isSwitch;
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

glm::vec2	Button::getPos()
{
	return glm::vec2(this->x_screen, this->y_screen);
}


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


void	Button::setSwitchMode(bool isSwitch)
{
	this->isSwitch = isSwitch;
}


void	Button::setPos(float x, float y)
{
	this->x_screen = x;
	this->y_screen = y;
}


void	Button::setColor(glm::vec3 color, int type)
{
	switch (type)
	{
		case BUTTON_BASE_COLOR_TYPE :
			this->baseColor = color;
			break;

		case BUTTON_UNDERLINE_COLOR_TYPE :
			this->baseColor = color;
			break;
	}
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
	glm::vec3 *					color;

	menuShader = shaderManager->getMenuShader();

	if (Button::mouse && this->active_B && this->mouseOnButton())
	{
		if (this->isSwitch && this->isON)
			color = &this->baseColor;
		else
			color = &this->underlineColor;
		if (Button::mouse->left.isPressed())
		{
			if (this->isSwitch)
				this->isON = !this->isON;
			this->press();
		}
	}
	else
	{
		if (this->isSwitch && this->isON)
			color = &this->underlineColor;
		else
			color = &this->baseColor;
	}
	points.push_back(Point2D(Vec2(x_screen, y_screen), (*color)[0], (*color)[1], (*color)[2]));
	points.push_back(Point2D(Vec2(x_screen + width, y_screen), (*color)[0], (*color)[1], (*color)[2]));
	points.push_back(Point2D(Vec2(x_screen, y_screen + height),  (*color)[0], (*color)[1], (*color)[2]));
	points.push_back(Point2D(Vec2(x_screen + width, y_screen + height), (*color)[0], (*color)[1], (*color)[2]));

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
	if (this->active_B && this->functionToExecute)
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
