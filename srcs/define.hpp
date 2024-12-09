/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2024/12/09 13:15:53 by lflandri         ###   ########.fr       */
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

std::vector<Vec3> parse(char *name);
std::vector<std::vector<double>> interpolate(std::vector<Vec3> point_list);


#endif
