/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functionnality.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 16:02:44 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/29 16:57:59 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <define.hpp>
#include <ui/interfaceDeclaration.hpp>
#include <model/WaterSimulation.hpp>
#include <model/Pannel.hpp>
#include <engine/maths/Random.hpp>


void	changeBoolStatus(void *arg)
{
	bool	*boolean = (bool *)arg;
	*boolean = !(*boolean);
}

void	updateRain(WaterSimulation *simulation, t_simulationVariable *sVar)
{

	static Random	random;
	for (size_t i = 0; i < sVar->rainIntensity; i++)
	{
		simulation->addWater(glm::vec3(random.random() * (MAP_SIZE - 1), WATER_MAX_HEIGHT,random.random() * (MAP_SIZE - 1)),
								glm::vec3(0.0f, 1.0f, 0.0f) * RAIN_VELOCITY);
	}
}

void	fillingPool(WaterSimulation *simulation, t_simulationVariable *sVar)
{

	static Random	random;
	for (size_t i = 0; i < sVar->fillingIntensity; i++)
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

void	generateAt(WaterSimulation *simulation, t_simulationVariable *sVar)
{
	static Random	random;

	for (size_t i = 0; i < sVar->generateIntensity; i++)
	{
		// std::cout << "deviationFactor : " << deviationFactor << std::endl;
		simulation->addWater(glm::vec3(sVar->generatePos.x + (random.random() * 2) - 1,
										sVar->generatePos.y + random.random(),
										sVar->generatePos.z + (random.random() * 2) - 1));
	}
}

void	generateWaveWest(void *arg)
{
	t_simulationVariable *sVar = ((t_simulationVariable *)(arg));
	WaterSimulation *simulation = ((WaterSimulation *)(sVar->simulation));

	for (float x = 0; x < MAP_SIZE; x += WATER_RADIUS * 2)
	{
		for (float y = 0; y < sVar->waveHeight; y += WATER_RADIUS * 2)
		{
			for (float z = 0; z < sVar->waveThickess; z += WATER_RADIUS * 2)
			{
				simulation->addWater(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 1.0f) * sVar->waveVelocity);

			}
		}
	}

}

void	generateWaveEst(void *arg)
{
	t_simulationVariable *sVar = ((t_simulationVariable *)(arg));
	WaterSimulation *simulation = ((WaterSimulation *)(sVar->simulation));

	for (float x = 0; x < MAP_SIZE; x += WATER_RADIUS * 2)
	{
		for (float y = 0; y < sVar->waveHeight; y += WATER_RADIUS * 2)
		{
			for (float z = MAP_SIZE - sVar->waveThickess - 1 ; z < MAP_SIZE; z += WATER_RADIUS * 2)
			{
				simulation->addWater(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, -1.0f) * sVar->waveVelocity);
			}
		}
	}

}

void	generateWaveNorth(void *arg)
{
	t_simulationVariable *sVar = ((t_simulationVariable *)(arg));
	WaterSimulation *simulation = ((WaterSimulation *)(sVar->simulation));

	for (float z = 0; z < MAP_SIZE; z += WATER_RADIUS * 2)
	{
		for (float y = 0; y < sVar->waveHeight; y += WATER_RADIUS * 2)
		{
			for (float x = 0; x < sVar->waveThickess; x += WATER_RADIUS * 2)
			{
				simulation->addWater(glm::vec3(x, y, z), glm::vec3(1.0f, 0.0f, 0.0f) * sVar->waveVelocity);

			}
		}
	}

}

void	generateWaveSouth(void *arg)
{
	t_simulationVariable *sVar = ((t_simulationVariable *)(arg));
	WaterSimulation *simulation = ((WaterSimulation *)(sVar->simulation));

	for (float z = 0; z < MAP_SIZE; z += WATER_RADIUS * 2)
	{
		for (float y = 0; y < sVar->waveHeight; y += WATER_RADIUS * 2)
		{
			for (float x = MAP_SIZE - sVar->waveThickess - 1 ; x < MAP_SIZE; x += WATER_RADIUS * 2)
			{
				simulation->addWater(glm::vec3(x, y, z), glm::vec3(-1.0f, 0.0f, 0.0f) * sVar->waveVelocity);
			}
		}
	}

}

void	generateWaveAll(void *arg)
{
	generateWaveEst(arg);
	generateWaveWest(arg);
	generateWaveNorth(arg);
	generateWaveSouth(arg);
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

void	moveRainPannel(void *arg)
{
	static bool isHide = true;
	Pannel *pannel = (Pannel *)arg;

	isHide = !isHide;
	if (isHide)
		pannel->addPosToGo(-120, 0);
	else
		pannel->addPosToGo(120, 0);
}

void	moveFillingPannel(void *arg)
{
	static bool isHide = true;
	Pannel *pannel = (Pannel *)arg;

	isHide = !isHide;
	if (isHide)
		pannel->addPosToGo(-120, 0);
	else
		pannel->addPosToGo(120, 0);
}

void	moveSettingsPannel(void *arg)
{
	static bool isHide = true;
	t_simulationVariable *sVar = ((t_simulationVariable *)(arg));

	isHide = !isHide;
	if (isHide)
	{
		sVar->pannelVector[5].addPosToGo(0, 300);
		sVar->pannelVector[7].addPosToGo(0, 170);
	}
	else
	{
		sVar->pannelVector[5].addPosToGo(0, -300);
		sVar->pannelVector[7].addPosToGo(0, -170);
	}
}

void	moveLightPannel(void *arg)
{
	static bool isHide = true;
	Pannel *pannel = (Pannel *)arg;

	isHide = !isHide;
	if (isHide)
		pannel->addPosToGo(-350, 0);
	else
		pannel->addPosToGo(350, 0);
}

void	moveGeneratePannel(void *arg)
{
	static bool isHide = true;
	Pannel *pannel = (Pannel *)arg;

	isHide = !isHide;
	if (isHide)
		pannel->addPosToGo(-240, 0);
	else
		pannel->addPosToGo(240, -0);
}

void	resetPool(void *arg)
{
	WaterSimulation *simulation = ((WaterSimulation *)(arg));
	simulation->clear();
}
