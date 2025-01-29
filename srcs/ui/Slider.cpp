/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Slider.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gugus <gugus@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 21:51:12 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/29 19:52:32 by gugus            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ui/Slider.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Mouse *Slider::mouse = NULL;
unsigned int Slider::texture = 0;

Slider::Slider(float x, float y, float width, float height)
:x_screen(x), y_screen(y), width(width), height(height)
{
	this->value = 0.0f;
	this->hasBeenClick = false;
	this->baseColor = DEFAULT_BUTTON_BASE_COLOR;
	this->underlineColor = DEFAULT_BUTTON_UNDERLINE_COLOR;
}


Slider::Slider(float x, float y, float width, float height, glm::vec3 baseColor, glm::vec3 underlineColor)
:x_screen(x), y_screen(y), width(width), height(height)
{
	this->value = 0.0f;
	this->hasBeenClick = false;
	this->baseColor = baseColor;
	this->underlineColor = underlineColor;
}


Slider::Slider(const Slider &obj)
:x_screen(obj.x_screen), y_screen(obj.y_screen), width(obj.width), height(obj.height)
{
	this->value = obj.value;
	this->hasBeenClick = obj.hasBeenClick;
	this->baseColor = obj.baseColor;
	this->underlineColor = obj.underlineColor;
}

//---- Destructor --------------------------------------------------------------

Slider::~Slider()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

glm::vec2	Slider::getPos()
{
	return glm::vec2(this->x_screen, this->y_screen);
}

//---- Setters -----------------------------------------------------------------

void	Slider::setValue(float value)
{
	this->value = value;
	if (value < 0.0f)
		this->value = 0.0f;
	if (value > 1.0f)
		this->value = 1.0f;
}


float	Slider::getValue()
{
	return this->value;
}


void	Slider::setPos(float x, float y)
{
	this->x_screen = x;
	this->y_screen = y;
}

//---- Operators ---------------------------------------------------------------

Slider	&Slider::operator=(const Slider &obj)
{
	if (this == &obj)
		return (*this);
	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Slider::renderMesh( ShaderManager *shaderManager)
{
	MenuShader					*menuShader;
	std::vector<Point2D>		points;
	std::vector<float>			vertices;
	const float WidthCursorDiv		= 0.1 * this->width / 2;
	unsigned int				indices[18] = {
												0, 1, 2,
												3, 1, 2,
												4, 5, 6,
												5, 6, 7,
												8, 9,10,
												9,10,11
												};
	float						texturePos[12][2] = {
													{0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f},
													{0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f},
													{0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f}
													};

	menuShader = shaderManager->getMenuShader();

	if (this->mouseOnSlider() && Slider::mouse->left.isDown())
		this->hasBeenClick = true;
	else if (this->hasBeenClick and !Slider::mouse->left.isDown())
		this->hasBeenClick = false;

	if (this->hasBeenClick)
	{
		this->setValue((Slider::mouse->getPos().x - this->x_screen) / this->width);
	}

	points.push_back(Point2D(Vec2(x_screen - WidthCursorDiv + this->width * this->value, y_screen + this->height / 2), this->baseColor[0], this->baseColor[1], this->baseColor[2]));
	points.push_back(Point2D(Vec2(x_screen + this->width * this->value, y_screen), DEFAULT_PANNEL_MIDDLE_CURSOR_COLOR[0], DEFAULT_PANNEL_MIDDLE_CURSOR_COLOR[1], DEFAULT_PANNEL_MIDDLE_CURSOR_COLOR[2]));
	points.push_back(Point2D(Vec2(x_screen + this->width * this->value, y_screen + this->height),  DEFAULT_PANNEL_MIDDLE_CURSOR_COLOR[0], DEFAULT_PANNEL_MIDDLE_CURSOR_COLOR[1], DEFAULT_PANNEL_MIDDLE_CURSOR_COLOR[2]));
	points.push_back(Point2D(Vec2(x_screen + WidthCursorDiv + this->width * this->value, y_screen + this->height / 2), this->underlineColor[0], this->underlineColor[1], this->underlineColor[2]));

	points.push_back(Point2D(Vec2(x_screen, y_screen), this->underlineColor[0], this->underlineColor[1], this->underlineColor[2]));
	points.push_back(Point2D(Vec2(x_screen + this->width * this->value, y_screen), this->underlineColor[0], this->underlineColor[1], this->underlineColor[2]));
	points.push_back(Point2D(Vec2(x_screen, y_screen + this->height),  this->underlineColor[0], this->underlineColor[1], this->underlineColor[2]));
	points.push_back(Point2D(Vec2(x_screen + this->width * this->value, y_screen + this->height), this->underlineColor[0], this->underlineColor[1], this->underlineColor[2]));

	points.push_back(Point2D(Vec2(x_screen + this->width * this->value, y_screen), this->baseColor[0], this->baseColor[1], this->baseColor[2]));
	points.push_back(Point2D(Vec2(x_screen + this->width, y_screen ), this->baseColor[0], this->baseColor[1], this->baseColor[2]));
	points.push_back(Point2D(Vec2(x_screen + this->width * this->value, y_screen + this->height),  this->baseColor[0], this->baseColor[1], this->baseColor[2]));
	points.push_back(Point2D(Vec2(x_screen + this->width, y_screen + this->height), this->baseColor[0], this->baseColor[1], this->baseColor[2]));

	for (int i = 0; i < 12; i++)
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 18, indices, GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Slider::texture);

	menuShader->use();

	glBindVertexArray(shaderManager->getVAOId());

	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
}

//**** PRIVATE METHODS *********************************************************

bool 	Slider::mouseOnSlider()
{
	const float WidthCursorDiv		= 0.1 * this->width / 2;

	if (Slider::mouse->getPos().x >= x_screen - WidthCursorDiv + this->width * this->value and
		Slider::mouse->getPos().x < x_screen + WidthCursorDiv + this->width * this->value and
		Slider::mouse->getPos().y >= this->y_screen and
		Slider::mouse->getPos().y < this->y_screen + this->height)
			return true;
	return false;
}
