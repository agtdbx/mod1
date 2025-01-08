/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gugus <gugus@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2025/01/08 14:18:17 by gugus            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_HPP
# define DEFINE_HPP

# include <iostream>
# include <GL/glew.h>
# include <GLFW/glfw3.h>
# include <glm/glm.hpp>
# include <vector>
# include <deque>

# include <engine/maths/Vec3.hpp>
# include <engine/maths/Vec2.hpp>

# define WIN_W 1600
# define WIN_H 900
// # define WIN_W 1920
// # define WIN_H 1080
# define WIN_TITLE "mod1"

# define MAX_XY 100
// # define MAX_HEIGHT 50
// # define MAP_SIZE 200
# define MAX_HEIGHT 40
# define MAP_SIZE 50
# define TERRAIN_PRECISION 0.01

# define CAMERA_FOV 45.0f
# define CAMERA_MIN_VIEW_DIST 0.1f
# define CAMERA_MAX_VIEW_DIST 1000.0f
# define CAMERA_SPEED 20.0
# define CAMERA_SPRINT_FACTOR 4.2
# define CAMERA_ROTATION_SPEED 42.0

# define GRAVITY_FORCE 10.0f
# define WATER_MASS 1.0f
# define COLLISION_ENERGY_KEEP 0.7
# define SMOOTHING_RADIUS 10.0f
# define TARGET_DENSITY 0.01f
# define PRESSURE_MULTIPLIER 100.0f
# define VISCOSITY_FORCE 2.0f
# define ENERGY_LOSE 0.97f;

# define PRINT_FPS_TIME 1.0f
# define MINIMUM_SIMULATION_UPDATE 0.1f

# define WORK_GROUP_SIZE 8

# define PI 3.1415926535f

const float		WIN_RATIO = (float)WIN_W/(float)WIN_H;
const float		MAP_MAX_HEIGHT = MAX_HEIGHT + 10;
const float		WATER_RADIUS = 0.5;
const float		WATER_RADIUS2 = WATER_RADIUS * WATER_RADIUS;
const float		WATER_MAX_XZ = MAP_SIZE - WATER_RADIUS;
const float		WATER_MAX_HEIGHT = MAP_MAX_HEIGHT - WATER_RADIUS;
const glm::vec3	WATER_COLOR = glm::vec3(0.0, 0.0, 0.8);
const float		SMOOTHING_SCALE = 15.0f / (2 * PI * pow(SMOOTHING_RADIUS, 5));
const float		SMOOTHING_DERIVATE_SCALE = 15.0f / (PI * pow(SMOOTHING_RADIUS, 5));
const float		SMOOTHING_VISCOSITY_SCALE = 315.0f / (64 * PI * pow(SMOOTHING_RADIUS, 9));

std::vector<Vec3>	parse(char *name);

#endif
