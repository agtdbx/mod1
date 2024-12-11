/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gugus <gugus@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2024/12/11 16:41:08 by gugus            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_HPP
# define DEFINE_HPP

# include <iostream>
# include <GL/glew.h>
# include <GLFW/glfw3.h>
# include <vector>
# include <engine/maths/Vec3.hpp>
# include <engine/maths/Vec2.hpp>

# define WIN_W 1600
# define WIN_H 900
# define WIN_TITLE "mod1"
# define MAP_SIZE 100
# define MAX_HEIGHT 210
# define CAMERA_SPEED 1.0
# define CAMERA_SPRINT_FACTOR 2.5
# define CAMERA_ROTATION_SPEED 42.0
# define TERRAIN_PRECISION 0.01

std::vector<Vec3>					parse(char *name);


#endif
