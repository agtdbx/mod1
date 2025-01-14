/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2025/01/14 22:14:29 by lflandri         ###   ########.fr       */
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
# define MAX_HEIGHT 50
# define MAP_SIZE 200
# define TERRAIN_PRECISION 0.01

# define CAMERA_FOV 45.0f
# define CAMERA_MIN_VIEW_DIST 0.1f
# define CAMERA_MAX_VIEW_DIST 1000.0f
# define CAMERA_SPEED 20.0
# define CAMERA_SPRINT_FACTOR 4.2
# define CAMERA_ROTATION_SPEED 42.0
# define CAMERA_ROTATION_SPEED_MOUSE 0.1

# define GRAVITY_FORCE 10.0f
# define WATER_MASS 1.0f
# define COLLISION_ENERGY_KEEP 0.5
# define SMOOTHING_RADIUS 5.0f
# define TARGET_DENSITY 0.01f
# define PRESSURE_MULTIPLIER 100.0f
# define VISCOSITY_FORCE 0.1f

# define RENDER_CELL_SIZE 20.0f

# define PRINT_FPS_TIME 1.0f
# define MINIMUM_SIMULATION_UPDATE 0.05f

# define WORK_GROUP_SIZE 8

# define PI 3.1415926535f

# define RAIN_INTENSITY 10
# define RAIN_VELOCITY 10.0f
# define RAIN_TIME_BEFORE_NEW_PARTICULE 0.1f
# define FILLING_TIME_BEFORE_NEW_PARTICULE 0.1f
# define FILLING_INTENSITY 10
# define FILLING_VELOCITY 20
# define WAVE_VELOCITY 42
# define WAVE_THICKNESS 10
# define WAVE_HEIGHT 10

# define COLOR_000000 glm::vec3(0.0f, 0.0f, 0.0f)
# define COLOR_29266F glm::vec3(0.160784314f, 0.149019608f, 0.435294118f)
# define COLOR_2C26E4 glm::vec3(0.17254902f, 0.149019608f, 0.894117647f)
# define COLOR_3EAFDF glm::vec3(0.243137255f, 0.68627451f, 0.874509804f)
# define COLOR_666666 glm::vec3(0.4f, 0.4f, 0.4f)
# define COLOR_6C3737 glm::vec3(0.423529412f, 0.215686275f, 0.215686275f)
# define COLOR_7F7F7F glm::vec3(0.5f, 0.5f, 0.5f)
# define COLOR_FFFFFF glm::vec3(1.0f, 1.0f, 1.0f)

# define DEFAULT_WATER_COLOR COLOR_3EAFDF
# define DEFAULT_PANNEL_COLOR COLOR_7F7F7F
# define DEFAULT_BUTTON_UNDERLINE_COLOR COLOR_666666
# define DEFAULT_BUTTON_BASE_COLOR COLOR_7F7F7F
# define DEFAULT_PANNEL_MIDDLE_CURSOR_COLOR COLOR_FFFFFF

# define PANNEL_COLOR COLOR_6C3737
# define PANNEL_MOVE_SPEED 200

const float		WIN_RATIO = (float)WIN_W/(float)WIN_H;
const float		MAP_MAX_HEIGHT = MAX_HEIGHT + 10;
const float		WATER_RADIUS = 1.0f;
const float		WATER_RADIUS2 = WATER_RADIUS * WATER_RADIUS;
const float		WATER_MAX_XZ = MAP_SIZE - WATER_RADIUS;
const float		WATER_MAX_HEIGHT = MAP_MAX_HEIGHT - WATER_RADIUS;
const glm::vec3	WATER_COLOR = glm::vec3(0.0, 0.0, 0.8);
const float		SMOOTHING_SCALE = 15.0f / (2 * PI * pow(SMOOTHING_RADIUS, 5));
const float		SMOOTHING_DERIVATE_SCALE = 15.0f / (PI * pow(SMOOTHING_RADIUS, 5));
const float		SMOOTHING_VISCOSITY_SCALE = 315.0f / (64 * PI * pow(SMOOTHING_RADIUS, 9));

std::vector<Vec3>	parse(char *name);






#endif
