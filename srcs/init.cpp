/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 19:45:12 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/24 15:08:08 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <engine/render/TextureManager.hpp>
#include <engine/render/shader/ShaderManager.hpp>
#include <interfaceDeclaratiom.hpp>
#include <model/Button.hpp>
#include <model/Pannel.hpp>
#include <model/Slider.hpp>
#include <model/TextEntry.hpp>

void	loadTexture(TextureManager *textureManager, ShaderManager *shaderManager)
{

		//number texture
		textureManager->addTexture("0", "data/textures/0.png");
		textureManager->addTexture("1", "data/textures/1.png");
		textureManager->addTexture("2", "data/textures/2.png");
		textureManager->addTexture("3", "data/textures/3.png");
		textureManager->addTexture("4", "data/textures/4.png");
		textureManager->addTexture("5", "data/textures/5.png");
		textureManager->addTexture("6", "data/textures/6.png");
		textureManager->addTexture("7", "data/textures/7.png");
		textureManager->addTexture("8", "data/textures/8.png");
		textureManager->addTexture("9", "data/textures/9.png");


		//terrain texture
		textureManager->addTexture("dirt", "data/textures/dirt.png");

		//multi texture
		textureManager->addTexture("active", "data/textures/active.png");
		textureManager->addTexture("noTexture", "data/textures/noTexture.png");

		//main pannel texture
		textureManager->addTexture("rain", "data/textures/rainButton.png");
		textureManager->addTexture("reset", "data/textures/resetButton.png");
		textureManager->addTexture("filling", "data/textures/fillingButton.png");
		textureManager->addTexture("generate", "data/textures/generate.png");
		textureManager->addTexture("wave", "data/textures/waveButton.png");
		textureManager->addTexture("settings", "data/textures/settings.png");

		//wave pannel texture
		textureManager->addTexture("North", "data/textures/North.png");
		textureManager->addTexture("South", "data/textures/South.png");
		textureManager->addTexture("West", "data/textures/West.png");
		textureManager->addTexture("Est", "data/textures/Est.png");
		textureManager->addTexture("all", "data/textures/all.png");
		textureManager->addTexture("waveThickness", "data/textures/waveThickness.png");
		textureManager->addTexture("waveHeight", "data/textures/waveHeight.png");
		textureManager->addTexture("waveVelocity", "data/textures/waveVelocity.png");

		//rain pannel texture
		textureManager->addTexture("rainIntensity", "data/textures/rainIntensity.png");
		textureManager->addTexture("rainDelay", "data/textures/rainDelay.png");

		//filling pannel texture
		textureManager->addTexture("fillingIntensity", "data/textures/fillingIntensity.png");
		textureManager->addTexture("fillingDelay", "data/textures/fillingDelay.png");

		//generate pannel texture
		textureManager->addTexture("generateIntensity", "data/textures/generateIntensity.png");
		textureManager->addTexture("generateDelay", "data/textures/generateDelay.png");
		textureManager->addTexture("X", "data/textures/X.png");
		textureManager->addTexture("Y", "data/textures/Y.png");
		textureManager->addTexture("Z", "data/textures/Z.png");

		//pause & step pannel texture
		textureManager->addTexture("pause", "data/textures/pause.png");
		textureManager->addTexture("step", "data/textures/step.png");

		//stettings pannel texture
		textureManager->addTexture("sensibility", "data/textures/sensibility.png");
		textureManager->addTexture("waterColor", "data/textures/waterColor.png");
		textureManager->addTexture("red", "data/textures/red.png");
		textureManager->addTexture("green", "data/textures/green.png");
		textureManager->addTexture("blue", "data/textures/blue.png");
		textureManager->addTexture("sprintSpeed", "data/textures/sprintSpeed.png");
		textureManager->addTexture("waterDensity", "data/textures/waterDensity.png");




		//terrain shader
		shaderManager->addShader("terrain", "data/shaders/terrain/terrain.glslv", "data/shaders/terrain/terrain.glslf");

		//watershader
		shaderManager->loadWaterShaderFiles("data/shaders/water/water.glslv", "data/shaders/water/water.glslf");
		shaderManager->loadWaterShaderDebugFiles("data/shaders/water/water.glslv", "data/shaders/water/waterBall.glslf");

		//menu shader
		shaderManager->loadMenuShaderFiles("data/shaders/menu.vs", "data/shaders/menu.fs");

		//compute shader
		shaderManager->addComputeShader("predictedPositions", "data/shaders/simulation/predictedPositions.glslc");
		shaderManager->addComputeShader("densities", "data/shaders/simulation/densities.glslc");
		shaderManager->addComputeShader("mapDensities", "data/shaders/simulation/mapDensities.glslc");
		shaderManager->addComputeShader("pressure", "data/shaders/simulation/pressure.glslc");
		shaderManager->addComputeShader("updatePositions", "data/shaders/simulation/updatePositions.glslc");
}

void	initUi(t_simulationVariable	*sVar, TextureManager *textureManager, WaterSimulation *simulation)
{
	//init
	sVar->fillingIntensity = FILLING_INTENSITY;
	sVar->fillingDelay = FILLING_TIME_BEFORE_NEW_PARTICULE;
	sVar->isFilling = false;
	sVar->isPannelHide = true;
	sVar->isRainning = false;
	sVar->rainIntensity = RAIN_INTENSITY;
	sVar->rainDelay = RAIN_TIME_BEFORE_NEW_PARTICULE;
	sVar->waveHeight = WAVE_HEIGHT;
	sVar->waveThickess = WAVE_THICKNESS;
	sVar->waveVelocity = WAVE_VELOCITY;
	sVar->isStopped = false;
	sVar->needStep = false;
	sVar->simulation = simulation;
	sVar->sprintSpeed = CAMERA_SPRINT_FACTOR;
	sVar->cameraSensibility = CAMERA_ROTATION_SPEED_MOUSE;
	sVar->watercolor = DEFAULT_WATER_COLOR;
	sVar->isGenerate = false;
	sVar->generateIntensity = GENERATE_INTENSITY;
	sVar->generateDelay = GENERATE_TIME_BEFORE_NEW_PARTICULE;
	sVar->generatePos = Vec3(GENERATE_START_POS);
	sVar->drawDebug = false;
	sVar->waterDensity = WATER_DENSITY;

	//pannel
	sVar->pannelVector.push_back(Pannel(WIN_W, 0.0f, 230, 250, textureManager->getTexture("noTexture"), PANNEL_COLOR));				//main pannel
	sVar->pannelVector.push_back(Pannel(WIN_W + 120, 255.0f, 120, 590, textureManager->getTexture("noTexture"), PANNEL_COLOR));		//wave pannel
	sVar->pannelVector.push_back(Pannel(0 - 240, 0.0f, 120, 310, textureManager->getTexture("noTexture"), PANNEL_COLOR));			//rain pannel
	sVar->pannelVector.push_back(Pannel(0 - 240, 320.0f, 120, 310, textureManager->getTexture("noTexture"), PANNEL_COLOR));			//filling pannel
	sVar->pannelVector.push_back(Pannel(WIN_W / 2 - 58, -58.0f, 112, 58, textureManager->getTexture("noTexture"), PANNEL_COLOR));	//stop & next step pannel
	sVar->pannelVector.push_back(Pannel(WIN_W / 2 - 175, WIN_H + 300, 350, 300, textureManager->getTexture("noTexture"), PANNEL_COLOR));	//settings pannel
	sVar->pannelVector.push_back(Pannel(0 - 480, 640, 240, 310, textureManager->getTexture("noTexture"), PANNEL_COLOR));	//generate pannel

	//main pannel content
	sVar->pannelVector[0].addButton(Button(10, 10, 100, 50,moveRainPannel, &sVar->pannelVector[2], textureManager->getTexture("rain")));
	sVar->pannelVector[0][0].setSwitchMode(true);
	sVar->pannelVector[0].addButton(Button(120, 10, 100, 50,moveFillingPannel, &sVar->pannelVector[3], textureManager->getTexture("filling")));
	sVar->pannelVector[0][1].setSwitchMode(true);
	sVar->pannelVector[0].addButton(Button(10, 70, 100, 50,moveWavePannel, &sVar->pannelVector[1], textureManager->getTexture("wave")));
	sVar->pannelVector[0][2].setSwitchMode(true);
	sVar->pannelVector[0].addButton(Button(70, 130, 100, 50,moveSettingsPannel, &sVar->pannelVector[5], textureManager->getTexture("settings")));
	sVar->pannelVector[0][3].setSwitchMode(true);
	sVar->pannelVector[0].addButton(Button(120, 70, 100, 50,moveGeneratePannel, &sVar->pannelVector[6], textureManager->getTexture("generate")));
	sVar->pannelVector[0][4].setSwitchMode(true);
	sVar->pannelVector[0].addButton(Button(70, 190, 100, 50,resetPool, simulation, textureManager->getTexture("reset")));

	//wave pannel content
	sVar->pannelVector[1].addButton(Button(10, 10, 100, 50,generateWaveNorth, sVar, textureManager->getTexture("North")));
	sVar->pannelVector[1].addButton(Button(10, 70, 100, 50,generateWaveWest, sVar, textureManager->getTexture("West")));
	sVar->pannelVector[1].addButton(Button(10, 130, 100, 50,generateWaveEst, sVar, textureManager->getTexture("Est")));
	sVar->pannelVector[1].addButton(Button(10, 190, 100, 50,generateWaveSouth, sVar, textureManager->getTexture("South")));
	sVar->pannelVector[1].addButton(Button(10, 250, 100, 50,generateWaveAll, sVar, textureManager->getTexture("all")));
	sVar->pannelVector[1].addButton(Button(10, 310, 100, 50,NULL, NULL, textureManager->getTexture("waveHeight"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[1][5].desactive();
	sVar->pannelVector[1].addSlider(Slider(10, 370, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[1][0.0f].setValue(0.5);
	sVar->pannelVector[1].addButton(Button(10, 400, 100, 50,NULL, NULL, textureManager->getTexture("waveVelocity"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[1][6].desactive();
	sVar->pannelVector[1].addSlider(Slider(10, 460, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[1][1.0f].setValue(0.5);
	sVar->pannelVector[1].addButton(Button(10, 490, 100, 50,NULL, NULL, textureManager->getTexture("waveThickness"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[1][7].desactive();
	sVar->pannelVector[1].addSlider(Slider(10, 560, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[1][2.0f].setValue(0.5);

	//rain pannel content
	sVar->pannelVector[2].addButton(Button(10, 10, 100, 50,NULL, NULL, textureManager->getTexture("rain"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[2][0].desactive();
	sVar->pannelVector[2].addButton(Button(10, 70, 100, 50,changeBoolStatus, &sVar->isRainning, textureManager->getTexture("active")));
	sVar->pannelVector[2][1].setSwitchMode(true);
	sVar->pannelVector[2].addButton(Button(10, 130, 100, 50,NULL, NULL, textureManager->getTexture("rainIntensity"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[2][2].desactive();
	sVar->pannelVector[2].addSlider(Slider(10, 190, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[2][0.0f].setValue(0.5);
	sVar->pannelVector[2].addButton(Button(10, 210, 100, 50,NULL, NULL, textureManager->getTexture("rainDelay"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[2][3].desactive();
	sVar->pannelVector[2].addSlider(Slider(10, 270, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[2][1.0f].setValue(0.5);

	//filling pannel content
	sVar->pannelVector[3].addButton(Button(10, 10, 100, 50,NULL, NULL, textureManager->getTexture("filling"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[3][0].desactive();
	sVar->pannelVector[3].addButton(Button(10, 70, 100, 50,changeBoolStatus, &sVar->isFilling, textureManager->getTexture("active")));
	sVar->pannelVector[3][1].setSwitchMode(true);
	sVar->pannelVector[3].addButton(Button(10, 130, 100, 50,NULL, NULL, textureManager->getTexture("fillingIntensity"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[3][2].desactive();
	sVar->pannelVector[3].addSlider(Slider(10, 190, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[3][0.0f].setValue(0.5);
	sVar->pannelVector[3].addButton(Button(10, 210, 100, 50,NULL, NULL, textureManager->getTexture("fillingDelay"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[3][3].desactive();
	sVar->pannelVector[3].addSlider(Slider(10, 270, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[3][1.0f].setValue(0.5);

	// pause & step pannel content
	sVar->pannelVector[4].addButton(Button(4, 4, 50, 50,changeBoolStatus, &sVar->isStopped, textureManager->getTexture("pause")));
	sVar->pannelVector[4][0].setSwitchMode(true);
	sVar->pannelVector[4].addButton(Button(58, 4, 50, 50,changeBoolStatus, &sVar->needStep, textureManager->getTexture("step")));

	// settings pannel content
	sVar->pannelVector[5].addButton(Button(125, 10, 100, 50,NULL, NULL, textureManager->getTexture("settings"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[5][0].desactive();
	sVar->pannelVector[5].addButton(Button(20, 60, 100, 50,NULL, NULL, textureManager->getTexture("sensibility"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[5][1].desactive();
	sVar->pannelVector[5].addSlider(Slider(20, 120, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[5][0.0f].setValue(0.5);
	sVar->pannelVector[5].addButton(Button(130, 60, 100, 50,NULL, NULL, textureManager->getTexture("sprintSpeed"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[5][2].desactive();
	sVar->pannelVector[5].addSlider(Slider(130, 120, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[5][1.0f].setValue(0.5);
	sVar->pannelVector[5].addButton(Button(100, 140, 150, 50,NULL, NULL, textureManager->getTexture("waterColor"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[5][3].desactive();
	sVar->pannelVector[5].addButton(Button(25, 180, 80, 50,NULL, NULL, textureManager->getTexture("red"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[5][4].desactive();
	sVar->pannelVector[5].addSlider(Slider(125, 200, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[5][2.0f].setValue(DEFAULT_WATER_COLOR[0]);
	sVar->pannelVector[5].addButton(Button(25, 220, 80, 50,NULL, NULL, textureManager->getTexture("green"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[5][5].desactive();
	sVar->pannelVector[5].addSlider(Slider(125, 240, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[5][3.0f].setValue(DEFAULT_WATER_COLOR[1]);
	sVar->pannelVector[5].addButton(Button(25, 260, 80, 50,NULL, NULL, textureManager->getTexture("blue"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[5][6].desactive();
	sVar->pannelVector[5].addSlider(Slider(125, 280, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[5][4.0f].setValue(DEFAULT_WATER_COLOR[2]);
	sVar->pannelVector[5].addButton(Button(235, 205, 80, 80,NULL, NULL, textureManager->getTexture("noTexture"), DEFAULT_WATER_COLOR, DEFAULT_WATER_COLOR));
	sVar->pannelVector[5][7].desactive();
	sVar->pannelVector[5].addButton(Button(240, 60, 100, 50,NULL, NULL, textureManager->getTexture("waterDensity"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[5][8].desactive();
	sVar->pannelVector[5].addSlider(Slider(240, 120, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[5][5.0f].setValue(0.5);

	//filling pannel content
	sVar->pannelVector[6].addButton(Button(70, 10, 100, 50,NULL, NULL, textureManager->getTexture("generate"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[6][0].desactive();
	sVar->pannelVector[6].addButton(Button(70, 70, 100, 50,changeBoolStatus, &sVar->isGenerate, textureManager->getTexture("active")));
	sVar->pannelVector[6][1].setSwitchMode(true);
	sVar->pannelVector[6].addTextEntry(TextEntry(40, 220, 30, 30));
	sVar->pannelVector[6][(char) 0].setValue(std::to_string((int) sVar->generatePos.x));
	sVar->pannelVector[6].addTextEntry(TextEntry(120, 220, 30, 30));
	sVar->pannelVector[6][(char) 1].setValue(std::to_string((int) sVar->generatePos.y));
	sVar->pannelVector[6].addTextEntry(TextEntry(200, 220, 30, 30));
	sVar->pannelVector[6][(char) 2].setValue(std::to_string((int) sVar->generatePos.z));
	sVar->pannelVector[6].addButton(Button(10, 130, 100, 50,NULL, NULL, textureManager->getTexture("generateIntensity"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[6][2].desactive();
	sVar->pannelVector[6].addSlider(Slider(10, 190, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[6][0.0f].setValue(0.5);
	sVar->pannelVector[6].addButton(Button(130, 130, 100, 50,NULL, NULL, textureManager->getTexture("generateDelay"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[6][3].desactive();
	sVar->pannelVector[6].addSlider(Slider(130, 190, 100, 10, COLOR_29266F, COLOR_2C26E4));
	sVar->pannelVector[6][1.0f].setValue(0.5);
	sVar->pannelVector[6].addButton(Button(10, 222, 20, 30,NULL, NULL, textureManager->getTexture("X"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[6][2].desactive();
	sVar->pannelVector[6].addButton(Button(90, 222, 20, 30,NULL, NULL, textureManager->getTexture("Y"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[6][3].desactive();
	sVar->pannelVector[6].addButton(Button(170, 222, 20, 30,NULL, NULL, textureManager->getTexture("Z"), PANNEL_COLOR, PANNEL_COLOR));
	sVar->pannelVector[6][4].desactive();
}
