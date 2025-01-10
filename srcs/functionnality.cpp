/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functionnality.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 16:02:44 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/10 23:20:36 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <define.hpp>
#include <model/WaterSimulation.hpp>
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

void	generateWave(void *arg)
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

void	resetPool(void *arg)
{
	WaterSimulation *simulation = ((WaterSimulation *)(arg));
	simulation->clear();
}