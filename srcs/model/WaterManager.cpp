/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WaterManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 15:25:10 by lflandri          #+#    #+#             */
/*   Updated: 2024/12/16 18:25:48 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "WaterManager.hpp"

WaterManager::WaterManager(/* args */)
{
	this->waterDrops = std::vector<Water>();
	this->waterMapPoint = std::vector<std::vector<std::vector<bool> >>(MAP_SIZE, std::vector<std::vector<bool> >(WATER_MAX_HEIGHT, std::vector<bool>(MAP_SIZE, 0)));
	this->pointSave = std::vector<Vec3>(MAP_SIZE * MAP_SIZE * MAX_HEIGHT, Vec3(0.0, 0.0, 0.0));


	// this->waterMapPoint = std::vector<std::vector<std::vector<bool> >>();
	// for (size_t x= 0; x < MAP_SIZE; x++)
	// {
	// 	this->waterMapPoint.push_back(std::vector<std::vector<bool> >());
	// 	for (size_t y = 0; y < WATER_MAX_HEIGHT; y++)
	// 	{
	// 		this->waterMapPoint[x].push_back(std::vector<bool>(MAP_SIZE, 0));
	// 	}
		
	// }
	
}

WaterManager::~WaterManager()
{
}

WaterManager::WaterManager (const WaterManager &obj)
{
	this->waterDrops = obj.waterDrops;
}
WaterManager& WaterManager::operator=(const WaterManager& x)
{
	this->waterDrops = x.waterDrops;
	return (*this);
}


std::vector<Water> & WaterManager::getWaterDrops()
{
	return this->waterDrops;
}
void	WaterManager::addWaterDrop(Water drop)
{
	this->waterDrops.push_back(drop);
}

void WaterManager::setWaterDrops(std::vector<Water> newDrops)
{
	this->waterDrops = newDrops;
}

void WaterManager::updatePosition()
{
	for(Water & water : this->waterDrops)
	{
		water.update();
		// water.actualiseMeshPosition();
	}
}

void WaterManager::updateDensity()
{
	//TODO multi-thread this shit :
	for(Water & water : this->waterDrops)
	{
		water.calculSelfDensity(this->waterDrops);
	}
}


void WaterManager::draw(Camera *camera, Shader *shader, TextureManager *textureManager, std::string textureName)
{
	// for(Water & water : this->waterDrops)
	// {
	// 	water.renderMesh(camera, shader, textureManager, textureName);
	// }


	// front face :                   back face :
	//                2    3                         6    7
	//
	//                0    1                         4    5
	std::vector<Point>		vertices;
	std::vector<t_tri_id>	indices;


	for (size_t x = 0; x < MAP_SIZE - 1; x++)
	{
		for (size_t y = 0; y < WATER_MAX_HEIGHT - 1; y++)
		{
			for (size_t z = 0; z < WATER_MAX_HEIGHT - 1; z++)
			{
				unsigned int choice = 0;

				if (this->waterMapPoint[x + 1][y + 1][z + 1])	//	7
					choice++;
				choice <<= 1;
				if (this->waterMapPoint[x][y + 1][z + 1])	//	6
					choice++;
				choice <<= 1;
				if (this->waterMapPoint[x + 1][y][z + 1])	//	5
					choice++;
				choice <<= 1;
				if (this->waterMapPoint[x][y][z + 1])	//	4
					choice++;
				choice <<= 1;
				if (this->waterMapPoint[x + 1][y + 1][z])	//	3
					choice++;
				choice <<= 1;
				if (this->waterMapPoint[x][y + 1][z])	//	2
					choice++;
				choice <<= 1;
				if (this->waterMapPoint[x + 1][y][z])	//	1
					choice++;
				choice <<= 1;
				if (this->waterMapPoint[x][y][z])	//	0
					choice++;
			}
		}
	}
	Mesh mesh = Mesh(vertices, indices);
	mesh.draw(camera, shader, textureManager, textureName);
}

void WaterManager::calculWaterInGrid()
{
	int ind = 0;
	// int pointSaveSize = this->pointSave.size();
	while (!(this->pointSave[ind].x == 0 && this->pointSave[ind].x == 0 && this->pointSave[ind].z == 0))
	{
		this->waterMapPoint[this->pointSave[ind].x][this->pointSave[ind].y][this->pointSave[ind].z] = 0;
		this->pointSave[ind].x = 0;
		this->pointSave[ind].y = 0;
		this->pointSave[ind].z = 0;
		ind++;
	}
	this->waterMapPoint[this->pointSave[0].x][this->pointSave[0].y][this->pointSave[0].z] = 0;
	ind = 0;
	for (Water & water : this->waterDrops)
	{
		Vec3 waterPos = Vec3(int(water.getPosition().x), int(water.getPosition().y), int(water.getPosition().z));
		
		this->waterMapPoint[waterPos.x][waterPos.y][waterPos.z] = 1;
		this->pointSave[ind].x = waterPos.x;
		this->pointSave[ind].y = waterPos.y;
		this->pointSave[ind].z = waterPos.z;
		ind++;
	}
}

