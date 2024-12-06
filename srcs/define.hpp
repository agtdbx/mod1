/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aderouba <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2024/12/06 13:48:33 by aderouba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_HPP
# define DEFINE_HPP

# include <iostream>
# include <GL/glew.h>
# include <GLFW/glfw3.h>

# define WIN_W 1600
# define WIN_H 900
# define WIN_TITLE "mod1"

typedef struct s_key
{
	int		keycode;
	bool	pressed;
	bool	released;
	bool	down;
}	t_key;

typedef struct s_mouse
{
	t_key	left;
	t_key	middle;
	t_key	right;
	double	x;
	double	y;
	double	scroll;
}	t_mouse;

typedef struct s_input_manager
{
	t_key	escape;
	t_key	e;
	t_mouse	mouse;
}	t_input_manager;


#endif
