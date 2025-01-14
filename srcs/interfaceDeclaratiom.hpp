/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interfaceDeclaratiom.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:06:48 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/14 16:31:57 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERFACEDECLARATION_HPP
# define INTERFACEDECLARATION_HPP

# include <model/Pannel.hpp>
# include <model/WaterSimulation.hpp>

typedef struct s_simulationVariable
{
	std::vector<Pannel> pannelVector;
	bool				isRainning;
	float				rainIntensity;
	float				rainDelay;
	bool				isFilling;
	float				fillingIntensity;
	float				fillingDelay;
	int					waveThickess;
	int					waveHeight;
	float				waveVelocity;
	bool				isPannelHide;
	void				*simulation;
}	t_simulationVariable;

void	addWater(void * arg);
void	changeBoolStatus(void *arg);
void	updateRain(WaterSimulation *simulation, t_simulationVariable *sVar);
void	fillingPool(WaterSimulation *simulation, t_simulationVariable *sVar);
void	generateWaveWest(void *arg);
void	generateWaveEst(void *arg);
void	generateWaveNorth(void *arg);
void	generateWaveSouth(void *arg);
void	generateWaveAll(void *arg);
void	moveWavePannel(void *arg);
void	moveRainPannel(void *arg);
void	moveFillingPannel(void *arg);
void	resetPool(void *arg);

#endif