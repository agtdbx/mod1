/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Water.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:55:32 by lflandri          #+#    #+#             */
/*   Updated: 2024/12/12 18:45:45 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Water.hpp"

Water::Water()
{
	this->velocity = Vec3(0.0, 0.0, 0.0);
	this->position = Vec3(0.0, 0.0, 0.0);
	this->density = 0.0;
	this->gravity = WATER_GRAVITY;

	this->vertices = std::vector<Point>();
	std::vector<t_tri_id>	indices;
	this->vertices.push_back(Point(this->position.x, this->position.y - WATER_DROP_SIZE, this->position.z, 0, 0, 255));

	this->vertices.push_back(Point(this->position.x + WATER_DROP_SIZE, this->position.y, this->position.z, 0, 255, 0));
	this->vertices.push_back(Point(this->position.x - WATER_DROP_SIZE, this->position.y, this->position.z + WATER_DROP_SIZE, 0, 255, 0));
	this->vertices.push_back(Point(this->position.x - WATER_DROP_SIZE, this->position.y, this->position.z - WATER_DROP_SIZE, 0, 255, 0));
	
	this->vertices.push_back(Point(this->position.x, this->position.y + WATER_DROP_SIZE, this->position.z, 0, 0, 255));

	indices.push_back((t_tri_id){0, 1, 2});
	indices.push_back((t_tri_id){0, 2, 3});
	indices.push_back((t_tri_id){0, 3, 1});
	indices.push_back((t_tri_id){4, 1, 2});
	indices.push_back((t_tri_id){4, 2, 3});
	indices.push_back((t_tri_id){4, 3, 1});

	this->mesh = Mesh(this->vertices, indices);

}

Water::Water(Vec3 pos, Vec3 vel, double grav)					//constructor
{
	this->velocity = vel;
	this->position = pos;
	this->gravity = grav;
	this->density = 0.0;

	this->vertices = std::vector<Point>();
	std::vector<t_tri_id>	indices;
	this->vertices.push_back(Point(this->position.x, this->position.y - WATER_DROP_SIZE, this->position.z, 0, 0, 255));

	this->vertices.push_back(Point(this->position.x + WATER_DROP_SIZE, this->position.y, this->position.z, 0, 255, 0));
	this->vertices.push_back(Point(this->position.x - WATER_DROP_SIZE, this->position.y, this->position.z + WATER_DROP_SIZE, 0, 255, 0));
	this->vertices.push_back(Point(this->position.x - WATER_DROP_SIZE, this->position.y, this->position.z - WATER_DROP_SIZE, 0, 255, 0));
	
	this->vertices.push_back(Point(this->position.x, this->position.y + WATER_DROP_SIZE, this->position.z, 0, 0, 255));

	indices.push_back((t_tri_id){0, 1, 2});
	indices.push_back((t_tri_id){0, 2, 3});
	indices.push_back((t_tri_id){0, 3, 1});
	indices.push_back((t_tri_id){4, 1, 2});
	indices.push_back((t_tri_id){4, 2, 3});
	indices.push_back((t_tri_id){4, 3, 1});

	this->mesh = Mesh(this->vertices, indices);

}

Water::~Water()
{
	
}

Water::Water (const Water &obj)
{
	this->velocity = obj.velocity;
	this->position = obj.position;
	this->gravity = obj.gravity;
	this->density = obj.density;

	this->vertices = std::vector<Point>();
	std::vector<t_tri_id>	indices;
	this->vertices.push_back(Point(this->position.x, this->position.y - WATER_DROP_SIZE, this->position.z, 0, 0, 255));

	this->vertices.push_back(Point(this->position.x + WATER_DROP_SIZE, this->position.y, this->position.z, 0, 255, 0));
	this->vertices.push_back(Point(this->position.x - WATER_DROP_SIZE, this->position.y, this->position.z + WATER_DROP_SIZE, 0, 255, 0));
	this->vertices.push_back(Point(this->position.x - WATER_DROP_SIZE, this->position.y, this->position.z - WATER_DROP_SIZE, 0, 255, 0));
	
	this->vertices.push_back(Point(this->position.x, this->position.y + WATER_DROP_SIZE, this->position.z, 0, 0, 255));

	indices.push_back((t_tri_id){0, 1, 2});
	indices.push_back((t_tri_id){0, 2, 3});
	indices.push_back((t_tri_id){0, 3, 1});
	indices.push_back((t_tri_id){4, 1, 2});
	indices.push_back((t_tri_id){4, 2, 3});
	indices.push_back((t_tri_id){4, 3, 1});

	this->mesh = Mesh(this->vertices, indices);

}
Water& Water::operator=(const Water& x)
{
	this->velocity = x.velocity;
	this->position = x.position;
	this->gravity = x.gravity;
	this->density = x.density;

	this->vertices = std::vector<Point>();
	std::vector<t_tri_id>	indices;
	this->vertices.push_back(Point(this->position.x, this->position.y - WATER_DROP_SIZE, this->position.z, 0, 0, 255));

	this->vertices.push_back(Point(this->position.x + WATER_DROP_SIZE, this->position.y, this->position.z, 0, 255, 0));
	this->vertices.push_back(Point(this->position.x - WATER_DROP_SIZE, this->position.y, this->position.z + WATER_DROP_SIZE, 0, 255, 0));
	this->vertices.push_back(Point(this->position.x - WATER_DROP_SIZE, this->position.y, this->position.z - WATER_DROP_SIZE, 0, 255, 0));
	
	this->vertices.push_back(Point(this->position.x, this->position.y + WATER_DROP_SIZE, this->position.z, 0, 0, 255));

	indices.push_back((t_tri_id){0, 1, 2});
	indices.push_back((t_tri_id){0, 2, 3});
	indices.push_back((t_tri_id){0, 3, 1});
	indices.push_back((t_tri_id){4, 1, 2});
	indices.push_back((t_tri_id){4, 2, 3});
	indices.push_back((t_tri_id){4, 3, 1});

	this->mesh = Mesh(this->vertices, indices);
	return (*this);
}

Vec3 Water::getPosition()
{
	return this->position;
}

Vec3 Water::getVelocity()
{
	return this->velocity;
}

double Water::getGravity()
{
	return this->gravity;
}

double Water::getDensity()
{
	return this->density;
}

Mesh & Water::getMesh()
{
	return this->mesh;
}


void Water::setPosition(Vec3 pos)
{
	this->position = pos;
}

void Water::setVelocity(Vec3 vel)
{
	this->velocity = vel;
}

void Water::setGravity(double grav)
{
	this->gravity = grav;
}

void Water::setMesh(Mesh newMesh)
{
	this->mesh = newMesh;
}

void Water::actualiseMeshPosition(void)
{
	this->vertices[0].pos.x = this->position.x; 
	this->vertices[0].pos.y = this->position.y - WATER_DROP_SIZE;
	this->vertices[0].pos.z = this->position.z;

	this->vertices[1].pos.x = this->position.x + WATER_DROP_SIZE;
	this->vertices[1].pos.y = this->position.y;
	this->vertices[1].pos.z = this->position.z;

	this->vertices[2].pos.x = this->position.x - WATER_DROP_SIZE;
	this->vertices[2].pos.y = this->position.y;
	this->vertices[2].pos.z = this->position.z + WATER_DROP_SIZE;

	this->vertices[3].pos.x = this->position.x - WATER_DROP_SIZE;
	this->vertices[3].pos.y = this->position.y;
	this->vertices[3].pos.z = this->position.z - WATER_DROP_SIZE;

	this->vertices[4].pos.x = this->position.x;
	this->vertices[4].pos.y = this->position.y + WATER_DROP_SIZE;
	this->vertices[4].pos.z = this->position.z;

	this->mesh.setVertices(vertices);
}

void Water::update(void)
{
	this->velocity +=  Vec3(0, -1, 0) * this->gravity;
	this->position += this->velocity;
	if (this->position.y <= 0) // floor touch
	{
		this->position.y = 0.0;
		this->velocity.y *= -1 * COLLISION_DAMPLING;
	}
}

void Water::calculSelfDensity(std::vector<Water> waterDrops)
{
	double density = 0;

	for (Water & water : waterDrops)
	{
		double dist = (water.getPosition() - this->position).getNorm();;
		// double dist = vecTemp.getNorm();
		double influence = smoothingKernel(SMOOTHING_RADIUS, dist);
		density += WATER_DROP_MASS * influence;
	}
	this->density = density;
}


void	Water::renderMesh(Camera *camera, Shader *shader, TextureManager *textureManager, std::string textureName)
{
	this->mesh.draw(camera, shader, textureManager, textureName);
}
