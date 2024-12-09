/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpolate.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 11:00:06 by lflandri          #+#    #+#             */
/*   Updated: 2024/12/09 13:41:17 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <define.hpp>
#include <algorithm>
#include <cmath>


static std::vector<double> createMapLine()
{
	std::vector<double> line;
	for (int i = 0; i < MAP_SIZE; i++)
		line.push_back(0.0);
	return line;
}

static std::vector<std::vector<double>> createMap()
{
	std::vector<std::vector<double> > heighmap;
	for (int i = 0; i < MAP_SIZE; i++)
		heighmap.push_back(createMapLine());
	return heighmap;
}

std::vector<std::vector<double>> interpolate(std::vector<Vec3> point_list)
{
	std::vector<std::vector<double> > heighmap = createMap();

	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			std::vector<double> possible_heights;
			for (Vec3 point: point_list)
			{
				double px = point.x;
				double py = point.y;
				double pz = point.z;

				double dist = std::sqrt((px - x)*(px - x) + (py - y)*(py - y));
				// std::cout << "dist = " << dist << std::endl;
				if (dist == 0)
				{
					possible_heights.push_back(pz);
					continue;
				}
				double val = std::max(0.0, pz*pz - dist*dist);
				// std::cout << "val = " << val << std::endl;
				if (val == 0)
					continue;
				val /= pz*pz;
				val = val*val*val;
				double new_val = val * pz;
				if (new_val <= 0.01)
					continue;
				// std::cout << "new val = " << new_val << std::endl;
				possible_heights.push_back(new_val);

			}
			if (possible_heights.size() == 0)
					continue;
			double new_val = *std::max_element(possible_heights.begin(), possible_heights.end());
			// std::cout << "new val = " << new_val << std::endl;
			heighmap[y][x] = new_val * 10;
		}
	}

	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			std::cout << heighmap[y][x] << " ";
		}
		std::cout << std::endl;
	}
	return heighmap;
}

