/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 16:43:13 by lflandri          #+#    #+#             */
/*   Updated: 2024/12/12 16:51:19 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <engine/maths/math_constants.hpp>

double smoothingKernel(double dist, double radius)
{
	if (dist >= radius)
		return 0.0;
	double volume = (pi * radius * radius * radius * radius) / 6;
	return (radius - dist) * (radius - dist) / volume;
}

double SmoothingKernelDerivative(double dist, double radius)
{
	if (dist >= radius)
		return 0;
	double scale = 12 / ((radius * radius *radius *radius) * pi);
	return ((dist - radius) * scale);
}

