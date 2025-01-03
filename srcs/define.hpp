/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aderouba <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2025/01/03 16:27:04 by aderouba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_HPP
# define DEFINE_HPP

# include <iostream>
# include <GL/glew.h>
# include <GLFW/glfw3.h>
# include <glm/glm.hpp>
# include <vector>

# include <engine/maths/Vec3.hpp>
# include <engine/maths/Vec2.hpp>

# define WIN_W 1600
# define WIN_H 900
// # define WIN_W 1920
// # define WIN_H 1080
# define WIN_TITLE "mod1"

# define MAX_XY 100
# define MAX_HEIGHT 50
# define MAP_SIZE 200
# define TERRAIN_PRECISION 0.01

# define CAMERA_FOV 45.0f
# define CAMERA_MIN_VIEW_DIST 0.1f
# define CAMERA_MAX_VIEW_DIST 1000.0f
# define CAMERA_SPEED 20.0
# define CAMERA_SPRINT_FACTOR 4.2
# define CAMERA_ROTATION_SPEED 42.0

# define WATER_GRAVITY 0.01
# define COLLISION_ENERGY_KEEP 0.7
# define SMOOTHING_RADIUS 25

# define PRINT_FPS_TIME 1.0

const float		WIN_RATIO = (float)WIN_W/(float)WIN_H;
const float		WATER_RADIUS = 0.5;
const float		WATER_RADIUS2 = WATER_RADIUS * WATER_RADIUS;
const float		WATER_MAX_HEIGHT = (MAX_HEIGHT + 10) - WATER_RADIUS;
const glm::vec3	WATER_COLOR = glm::vec3(0.0, 0.0, 0.8);

std::vector<Vec3>	parse(char *name);

#endif
