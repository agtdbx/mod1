/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uiTick.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 20:28:30 by lflandri          #+#    #+#             */
/*   Updated: 2025/02/05 00:32:57 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <define.hpp>
#include <ui/interfaceDeclaration.hpp>

#include <engine/OpenGLContext.hpp>
#include <model/Terrain.hpp>
#include <ui/Button.hpp>
#include <ui/Pannel.hpp>
#include <ui/Slider.hpp>
#include <ui/TextEntry.hpp>

void	uiTick(t_simulationVariable *sVar)
{
	//wave parameter
	sVar->waveHeight =  sVar->pannelVector[1][0.0f].getValue() * 2 * WAVE_HEIGHT;
	sVar->waveVelocity =  sVar->pannelVector[1][1.0f].getValue() * 2 * WAVE_VELOCITY;
	sVar->waveThickess =  sVar->pannelVector[1][2.0f].getValue() * 2 * WAVE_THICKNESS;

	//rain parameter
	sVar->rainIntensity =  sVar->pannelVector[2][0.0f].getValue() * 2 * RAIN_INTENSITY;
	sVar->rainDelay =  sVar->pannelVector[2][1.0f].getValue() * 2 * RAIN_TIME_BEFORE_NEW_PARTICULE;

	//filling parameter
	sVar->fillingIntensity =  sVar->pannelVector[3][0.0f].getValue() * 2 * FILLING_INTENSITY;
	sVar->fillingDelay =  sVar->pannelVector[3][1.0f].getValue() * 2 * FILLING_TIME_BEFORE_NEW_PARTICULE;

	//settings parameter
	sVar->cameraSensibility = sVar->pannelVector[5][0.0f].getValue() * 2 *  CAMERA_ROTATION_SPEED_MOUSE;
	sVar->sprintSpeed = sVar->pannelVector[5][1.0f].getValue() * 2 *  CAMERA_SPRINT_FACTOR;
	sVar->watercolor = glm::vec3(sVar->pannelVector[5][2.0f].getValue(),
								sVar->pannelVector[5][3.0f].getValue(),
								sVar->pannelVector[5][4.0f].getValue());
	sVar->waterDensity = sVar->pannelVector[5][5.0f].getValue() * (2) *  WATER_DENSITY;
	sVar->pannelVector[5][7].setColor(sVar->watercolor, BUTTON_BASE_COLOR_TYPE);

	//generate parameter
	if (sVar->pannelVector[6][(char) 0].getValue().size())
	{
		sVar->generatePos.x = std::stoi(sVar->pannelVector[6][(char) 0].getValue());
		if (sVar->generatePos.x >= MAP_SIZE)
			sVar->generatePos.x = GENERATE_MAX_X;
	}
	else
	{
		sVar->generatePos.x = GENERATE_MIN_X;
	}
	if (sVar->pannelVector[6][(char) 1].getValue().size())
	{
		sVar->generatePos.y = std::stoi(sVar->pannelVector[6][(char)1].getValue());
		if (sVar->generatePos.y > MAP_MAX_HEIGHT)
			sVar->generatePos.y = MAP_MAX_HEIGHT;
	}
	else
	{
		sVar->generatePos.y = GENERATE_PADDING;
	}
	if (sVar->pannelVector[6][(char) 2].getValue().size())
	{
		sVar->generatePos.z = std::stoi(sVar->pannelVector[6][(char) 2].getValue());
		if (sVar->generatePos.z >= MAP_SIZE)
			sVar->generatePos.z = GENERATE_MAX_Z;
	}
	else
	{
		sVar->generatePos.z = GENERATE_MIN_Z;
	}
	sVar->generateIntensity =  sVar->pannelVector[6][0.0f].getValue() * 2 * GENERATE_INTENSITY;
	sVar->generateDelay =  sVar->pannelVector[6][1.0f].getValue() * 2 * GENERATE_TIME_BEFORE_NEW_PARTICULE;

	//light parameter
	sVar->lightcolor = glm::vec3(sVar->pannelVector[7][0.0f].getValue(),
								sVar->pannelVector[7][1.0f].getValue(),
								sVar->pannelVector[7][2.0f].getValue());
	sVar->pannelVector[7][4].setColor(sVar->lightcolor, BUTTON_BASE_COLOR_TYPE);


	//blackHole parameter
	sVar->holeInfo.radius =  sVar->pannelVector[8][0.0f].getValue() * 4 * BLACK_HOLE_RADIUS;
	if (sVar->pannelVector[8][(char) 0].getValue().size())
	{
		sVar->holeInfo.position.x = std::stoi(sVar->pannelVector[8][(char) 0].getValue());
		if (sVar->holeInfo.position.x >= MAP_SIZE)
			sVar->holeInfo.position.x = MAP_SIZE - BLACK_HOLE_RADIUS;
	}
	else
	{
		sVar->holeInfo.position.x = BLACK_HOLE_RADIUS;
	}
	if (sVar->pannelVector[8][(char) 1].getValue().size())
	{
		sVar->holeInfo.position.y = std::stoi(sVar->pannelVector[8][(char) 1].getValue());
		if (sVar->holeInfo.position.y >= MAP_SIZE)
			sVar->holeInfo.position.y = MAP_SIZE - BLACK_HOLE_RADIUS;
	}
	else
	{
		sVar->holeInfo.position.y = BLACK_HOLE_RADIUS;
	}
	
}
