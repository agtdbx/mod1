/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WaterManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 15:25:10 by lflandri          #+#    #+#             */
/*   Updated: 2024/12/16 14:26:53 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "WaterManager.hpp"

WaterManager::WaterManager(/* args */)
{
	this->waterDrops = std::vector<Water>();
	this->waterMapPoint = std::vector<std::vector<std::vector<bool> >>(MAP_SIZE, std::vector<std::vector<bool> >(WATER_MAX_HEIGHT, std::vector<bool>(MAP_SIZE, 0)));

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
		water.actualiseMeshPosition();
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
	for(Water & water : this->waterDrops)
	{
		water.renderMesh(camera, shader, textureManager, textureName);
	}	
}
