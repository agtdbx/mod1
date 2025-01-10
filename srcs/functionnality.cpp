/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functionnality.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 16:02:44 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/10 17:54:49 by lflandri         ###   ########.fr       */
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
	// std::cout << "Rainning" << std::endl; 
	// WaterSimulation *simulation = ((WaterSimulation *)(arg));
	static Random	random;
	for (size_t i = 0; i < RAIN_INTENSITY; i++)
	{
		simulation->addWater(glm::vec3(random.random() * MAP_SIZE, WATER_MAX_HEIGHT,random.random() * MAP_SIZE));
	}
}

void	fillingPool(WaterSimulation *simulation)
{
	// WaterSimulation *simulation = ((WaterSimulation *)(arg));
	// std::cout << "Filling" << std::endl; 
	static Random	random;
	for (size_t i = 0; i < FILLING_INTENSITY; i++)
	{
		simulation->addWater(glm::vec3(1.0f + random.random(),				i + WATER_RADIUS,	1.0f + random.random()));
		simulation->addWater(glm::vec3(MAP_SIZE - 1.0f - random.random(),	i + WATER_RADIUS,	1.0f + random.random()));
		simulation->addWater(glm::vec3(1.0f + random.random(),				i + WATER_RADIUS,	MAP_SIZE - 1.0f - random.random()));
		simulation->addWater(glm::vec3(MAP_SIZE - 1.0f - random.random(),	i + WATER_RADIUS,	MAP_SIZE - 1.0f - random.random()));
	}
	

}

void	generateWave(void *arg)
{
	WaterSimulation *simulation = ((WaterSimulation *)(arg));
	
}

void	resetPool(void *arg)
{
	WaterSimulation *simulation = ((WaterSimulation *)(arg));
	simulation->clear();
}