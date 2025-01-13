/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pannel.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <liam.flandrinck.58@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 21:51:16 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/13 14:17:26 by lflandri         ###   ########.fr       */
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
:x_screen(x), y_screen(y), width(width), height(height), x_toGo(x), y_toGo(y)
{
	this->padding = 0;
	this->texture = texture;
	this->baseColor = DEFAULT_PANNEL_COLOR;
}

Pannel::Pannel(float x, float y, float width, float height, unsigned int texture, glm::vec3 baseColor)
:x_screen(x), y_screen(y), width(width), height(height), x_toGo(x), y_toGo(y)
{
	this->padding = 0;
	this->texture = texture;
	this->baseColor = baseColor;
}



Pannel::Pannel(const Pannel &obj)
:x_screen(obj.x_screen), y_screen(obj.y_screen), width(obj.width), height(obj.height), x_toGo(obj.x_toGo), y_toGo(obj.y_toGo)
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

void	Pannel::setPosToGo(float x, float y)
{
	this->x_toGo = x;
	this->y_toGo = y;
}

void	Pannel::addPosToGo(float x, float y)
{
	this->x_toGo += x;
	this->y_toGo += y;
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
	// std::cout << "draw pos : " << this->x_screen << " | " << this->y_screen << std::endl;

	for (Button & button : this->buttonList)
	{
		button.renderMesh(shaderManager);
	}

	menuShader = shaderManager->getMenuShader();

	points.push_back(Point2D(Vec2(this->x_screen, this->y_screen), (*color)[0], (*color)[1], (*color)[2]));
	points.push_back(Point2D(Vec2(this->x_screen + this->width, this->y_screen), (*color)[0], (*color)[1], (*color)[2]));
	points.push_back(Point2D(Vec2(this->x_screen, this->y_screen + this->height),  (*color)[0], (*color)[1], (*color)[2]));
	points.push_back(Point2D(Vec2(this->x_screen + this->width, this->y_screen + this->height), (*color)[0], (*color)[1], (*color)[2]));

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

void	Pannel::tick(double delta)
{
	if (this->x_toGo == this->x_screen && this->y_toGo == this->y_screen)
		return ;
	glm::vec2	velocity(0.0f, 0.0f);

	if (this->x_toGo < this->x_screen)
		velocity[0] = -1.0f;
	else if (this->x_toGo > this->x_screen)
		velocity[0] = 1.0f;
	if (this->y_toGo < this->y_screen)
		velocity[1] = -1.0f;
	else if (this->y_toGo > this->y_screen)
		velocity[1] = 1.0f;

	velocity *= PANNEL_MOVE_SPEED * delta;
	if ((this->x_toGo < this->x_screen && this->x_screen + velocity[0]  < this->x_toGo)
		|| (this->x_toGo > this->x_screen && this->x_screen + velocity[0]  > this->x_toGo))
			velocity[0] = this->x_toGo - this->x_screen;
	if ((this->y_toGo < this->y_screen && this->y_screen + velocity[1]  < this->y_toGo)
		|| (this->y_toGo > this->y_screen && this->y_screen + velocity[1]  > this->y_toGo))
			velocity[1] = this->y_toGo - this->y_screen;
	this->x_screen += velocity[0]; 
	this->y_screen += velocity[1];
	this->moveButton(velocity[0], velocity[1]);
}


//**** PRIVATE METHODS *********************************************************

void	Pannel::moveButton(float mvx, float mvy)
{
	for (Button & button : this->buttonList)
	{
		button.setPos(button.getPos().x + mvx, button.getPos().y + mvy);
	}
}

