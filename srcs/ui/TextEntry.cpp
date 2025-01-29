/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextEntry.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gugus <gugus@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 21:51:12 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/29 19:52:50 by gugus            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ui/TextEntry.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Mouse *TextEntry::mouse = NULL;
InputManager *TextEntry::inputManager = NULL;
TextureManager *TextEntry::textureManager = NULL;

TextEntry::TextEntry(float x, float y, float width, float height)
:x_screen(x), y_screen(y), width(width), height(height)
{
	this->value = "";
	this->baseColor = DEFAULT_BUTTON_BASE_COLOR;
	this->underlineColor = DEFAULT_BUTTON_UNDERLINE_COLOR;
	this->isSelected = false;
}


TextEntry::TextEntry(float x, float y, float width, float height, glm::vec3 baseColor, glm::vec3 underlineColor)
:x_screen(x), y_screen(y), width(width), height(height)
{
	this->value = "";
	this->baseColor = baseColor;
	this->underlineColor = underlineColor;
	this->isSelected = false;
}


TextEntry::TextEntry(const TextEntry &obj)
:x_screen(obj.x_screen), y_screen(obj.y_screen), width(obj.width), height(obj.height)
{
	this->value = obj.value;
	this->baseColor = obj.baseColor;
	this->underlineColor = obj.underlineColor;
	this->isSelected = false;
}

//---- Destructor --------------------------------------------------------------

TextEntry::~TextEntry()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

glm::vec2	TextEntry::getPos()
{
	return glm::vec2(this->x_screen, this->y_screen);
}


std::string	TextEntry::getValue()
{
	return this->value;
}

//---- Setters -----------------------------------------------------------------

void	TextEntry::setValue(std::string value)
{
	this->value = value;
}


void	TextEntry::setPos(float x, float y)
{
	this->x_screen = x;
	this->y_screen = y;
}


void	TextEntry::setColor(glm::vec3 color, int type)
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

TextEntry	&TextEntry::operator=(const TextEntry &obj)
{
	if (this == &obj)
		return (*this);
	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	TextEntry::renderMesh( ShaderManager *shaderManager)
{
	MenuShader		*menuShader;
	unsigned int	indices[6] = {0, 1, 2, 3, 1, 2};
	float			texturePos[4][2] = {{0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f}};
	glm::vec3 *		color;

	menuShader = shaderManager->getMenuShader();

	if (this->mouseOnTextEntry() && TextEntry::mouse->left.isPressed())
		this->isSelected = true;
	else if (TextEntry::mouse->left.isPressed())
		this->isSelected = false;

	if (this->isSelected)
	{
		color = &this->underlineColor;
		if (TextEntry::inputManager->_0.isPressed())
			this->addCharToValue('0');
		if (TextEntry::inputManager->_1.isPressed())
			this->addCharToValue('1');
		if (TextEntry::inputManager->_2.isPressed())
			this->addCharToValue('2');
		if (TextEntry::inputManager->_3.isPressed())
			this->addCharToValue('3');
		if (TextEntry::inputManager->_4.isPressed())
			this->addCharToValue('4');
		if (TextEntry::inputManager->_5.isPressed())
			this->addCharToValue('5');
		if (TextEntry::inputManager->_6.isPressed())
			this->addCharToValue('6');
		if (TextEntry::inputManager->_7.isPressed())
			this->addCharToValue('7');
		if (TextEntry::inputManager->_8.isPressed())
			this->addCharToValue('8');
		if (TextEntry::inputManager->_9.isPressed())
			this->addCharToValue('9');
		if (TextEntry::inputManager->del.isPressed())
			this->removeLastCharFromValue();
	}
	else
	{
		color = &this->baseColor;
	}
	for (int i = 0; i < (int)this->value.size(); i++)
	{
		std::vector<Point2D>		points;
		std::vector<float>			vertices;

		points.push_back(Point2D(Vec2(x_screen + i * ENTRYTEXT_WITDH_CHAR, y_screen), (*color)[0], (*color)[1], (*color)[2]));
		points.push_back(Point2D(Vec2(x_screen + (i + 1) * ENTRYTEXT_WITDH_CHAR, y_screen), (*color)[0], (*color)[1], (*color)[2]));
		points.push_back(Point2D(Vec2(x_screen + i * ENTRYTEXT_WITDH_CHAR, y_screen + height),  (*color)[0], (*color)[1], (*color)[2]));
		points.push_back(Point2D(Vec2(x_screen + (i + 1) * ENTRYTEXT_WITDH_CHAR, y_screen + height), (*color)[0], (*color)[1], (*color)[2]));

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
		glBindTexture(GL_TEXTURE_2D, TextEntry::textureManager->getTexture(this->value.substr(i, 1)));


		menuShader->use();

		glBindVertexArray(shaderManager->getVAOId());

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	std::vector<Point2D>		points;
	std::vector<float>			vertices;
	points.push_back(Point2D(Vec2(x_screen + this->value.size() * ENTRYTEXT_WITDH_CHAR, y_screen), (*color)[0], (*color)[1], (*color)[2]));
	points.push_back(Point2D(Vec2(x_screen + width, y_screen), (*color)[0], (*color)[1], (*color)[2]));
	points.push_back(Point2D(Vec2(x_screen + this->value.size() * ENTRYTEXT_WITDH_CHAR, y_screen + height),  (*color)[0], (*color)[1], (*color)[2]));
	points.push_back(Point2D(Vec2(x_screen +  + width, y_screen + height), (*color)[0], (*color)[1], (*color)[2]));

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
	glBindTexture(GL_TEXTURE_2D, TextEntry::textureManager->getTexture("noTexture"));

	menuShader->use();

	glBindVertexArray(shaderManager->getVAOId());

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

//**** PRIVATE METHODS *********************************************************

bool 	TextEntry::mouseOnTextEntry()
{
	if (TextEntry::mouse->getPos().x >= this->x_screen && TextEntry::mouse->getPos().x < this->x_screen + this->width &&
		TextEntry::mouse->getPos().y >= this->y_screen && TextEntry::mouse->getPos().y < this->y_screen + this->height)
		return true;
	return false;
}


void	TextEntry::addCharToValue(char c)
{
	if ((this->value.size() + 1) * ENTRYTEXT_WITDH_CHAR <= this->width)
		this->value += c;
}


void	TextEntry::removeLastCharFromValue()
{
	if (this->value.size())
		this->value = this->value.substr(0, this->value.size() - 1);
}
