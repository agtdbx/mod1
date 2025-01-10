/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pannel.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 21:51:16 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/10 23:07:53 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <model/Pannel.hpp>

// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
// #include <stdexcept>

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Mouse *Pannel::mouse = NULL;

Pannel::Pannel(float x, float y, float width, float height, unsigned int texture)
:x_screen(x), y_screen(y), width(width), height(height)
{
	this->padding = 0;
	this->texture = texture;
	this->baseColor = DEFAULT_PANNEL_COLOR;
}

Pannel::Pannel(float x, float y, float width, float height, unsigned int texture, glm::vec3 baseColor)
:x_screen(x), y_screen(y), width(width), height(height)
{
	this->padding = 0;
	this->texture = texture;
	this->baseColor = baseColor;
}



Pannel::Pannel(const Pannel &obj)
:x_screen(obj.x_screen), y_screen(obj.y_screen), width(obj.width), height(obj.height)
{
	this->padding = obj.padding;
	this->texture = obj.texture;
	this->baseColor = obj.baseColor;
	buttonList = obj.buttonList;
}

//---- Destructor --------------------------------------------------------------

Pannel::~Pannel()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------


//---- Setters -----------------------------------------------------------------

void	Pannel::setPos(float x, float y)
{
	this->x_screen = x;
	this->y_screen = y;
}


//---- Operators ---------------------------------------------------------------

Pannel	&Pannel::operator=(const Pannel &obj)
{
	if (this == &obj)
		return (*this);
	return (*this);
}


Button& Pannel::operator[](int index)
{
    if (index >= this->buttonList.size() || index < 0)
	{
		throw std::range_error("Pannel error : can't get button of gived index");
    }
    return this->buttonList[index];
}

//**** PUBLIC METHODS **********************************************************



void	Pannel::renderMesh( ShaderManager *shaderManager)
{
	MenuShader					*menuShader;
	std::vector<Point2D>		points;
	std::vector<float>			vertices;
	unsigned int				indices[6] = {0, 1, 2, 3, 1, 2};
	float						texturePos[4][2] = {{0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f}};
	glm::vec3 *					color = &this->baseColor;

	for (Button & button : this->buttonList)
	{
		button.renderMesh(shaderManager);
	}

	menuShader = shaderManager->getMenuShader();

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

void	Pannel::addButton(Button  b)
{
	unsigned int indButton = this->buttonList.size();
	this->buttonList.push_back(b);
	Button & newButton = this->buttonList[indButton];
	glm::vec2 posButton= newButton.getPos();
	newButton.setPos(posButton[0] + this->x_screen + this->padding, posButton[1] + this->y_screen + this->padding);
}


//**** PRIVATE METHODS *********************************************************

