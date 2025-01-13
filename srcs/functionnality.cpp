/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functionnality.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <liam.flandrinck.58@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 16:02:44 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/13 14:53:22 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <define.hpp>
#include <model/WaterSimulation.hpp>
#include <model/Pannel.hpp>
#include <engine/maths/Random.hpp>


void	changeBoolStatus(void *arg)
{
	bool	*boolean = (bool *)arg;
	*boolean = !(*boolean);
}

void	updateRain(WaterSimulation *simulation)
{

	static Random	random;
	for (size_t i = 0; i < RAIN_INTENSITY; i++)
	{
		simulation->addWater(glm::vec3(random.random() * MAP_SIZE, WATER_MAX_HEIGHT,random.random() * MAP_SIZE));
	}
}

void	fillingPool(WaterSimulation *simulation)
{

	static Random	random;
	for (size_t i = 0; i < FILLING_INTENSITY; i++)
	{
		float deviationFactor = (random.random() - 0.5) * (FILLING_VELOCITY/2) * 5;
		// std::cout << "deviationFactor : " << deviationFactor << std::endl;
		simulation->addWater(glm::vec3(1.0f + random.random(),				i + WATER_RADIUS,	1.0f + random.random()),
							 glm::vec3(FILLING_VELOCITY + deviationFactor, 0.0f, FILLING_VELOCITY - deviationFactor));

		simulation->addWater(glm::vec3(MAP_SIZE - 1.0f - random.random(),	i + WATER_RADIUS,	1.0f + random.random()),
							 glm::vec3(-FILLING_VELOCITY + deviationFactor, 0.0f, FILLING_VELOCITY + deviationFactor));

		simulation->addWater(glm::vec3(1.0f + random.random(),				i + WATER_RADIUS,	MAP_SIZE - 1.0f - random.random()),
							 glm::vec3(FILLING_VELOCITY + deviationFactor, 0.0f, -FILLING_VELOCITY + deviationFactor));

		simulation->addWater(glm::vec3(MAP_SIZE - 1.0f - random.random(),	i + WATER_RADIUS,	MAP_SIZE - 1.0f - random.random()),
							 glm::vec3(-FILLING_VELOCITY + deviationFactor, 0.0f, -FILLING_VELOCITY - deviationFactor));

	}
	

}

void	generateWaveWest(void *arg)
{
	WaterSimulation *simulation = ((WaterSimulation *)(arg));
	
	for (float x = 0; x < MAP_SIZE; x += WATER_RADIUS * 2)
	{
		for (float y = 0; y < WAVE_HEIGHT; y += WATER_RADIUS * 2)
		{
			for (float z = 0; z < WAVE_THICKNESS; z += WATER_RADIUS * 2)
			{
				simulation->addWater(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 42.0f));
			
			}
		}
	}
	
}

void	generateWaveEst(void *arg)
{
	WaterSimulation *simulation = ((WaterSimulation *)(arg));

	for (float x = 0; x < MAP_SIZE; x += WATER_RADIUS * 2)
	{
		for (float y = 0; y < WAVE_HEIGHT; y += WATER_RADIUS * 2)
		{
			for (float z = MAP_SIZE - WAVE_THICKNESS - 1 ; z < MAP_SIZE; z += WATER_RADIUS * 2)
			{
				simulation->addWater(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, -42.0f));
			}
		}
	}
	
}

void	generateWaveNorth(void *arg)
{
	WaterSimulation *simulation = ((WaterSimulation *)(arg));
	
	for (float z = 0; z < MAP_SIZE; z += WATER_RADIUS * 2)
	{
		for (float y = 0; y < WAVE_HEIGHT; y += WATER_RADIUS * 2)
		{
			for (float x = 0; x < WAVE_THICKNESS; x += WATER_RADIUS * 2)
			{
				simulation->addWater(glm::vec3(x, y, z), glm::vec3(42.0f, 0.0f, 0.0f));
			
			}
		}
	}
	
}

void	generateWaveSouth(void *arg)
{
	WaterSimulation *simulation = ((WaterSimulation *)(arg));

	for (float z = 0; z < MAP_SIZE; z += WATER_RADIUS * 2)
	{
		for (float y = 0; y < WAVE_HEIGHT; y += WATER_RADIUS * 2)
		{
			for (float x = MAP_SIZE - WAVE_THICKNESS - 1 ; x < MAP_SIZE; x += WATER_RADIUS * 2)
			{
				simulation->addWater(glm::vec3(x, y, z), glm::vec3(-42.0f, 0.0f, 0.0f));
			}
		}
	}
	
}

void	moveWavePannel(void *arg)
{
	static bool isHide = true;
	Pannel *pannel = (Pannel *)arg;

	isHide = !isHide;
	if (isHide)
		pannel->addPosToGo(120, 0);
	else
		pannel->addPosToGo(-120, 0);
}

void	resetPool(void *arg)
{
	WaterSimulation *simulation = ((WaterSimulation *)(arg));
	simulation->clear();
}