/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WaterManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 15:25:10 by lflandri          #+#    #+#             */
/*   Updated: 2024/12/12 17:05:17 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "WaterManager.hpp"

WaterManager::WaterManager(/* args */)
{
	this->waterDrops = std::vector<Water>();
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
