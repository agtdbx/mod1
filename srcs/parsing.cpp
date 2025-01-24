/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aderouba <aderouba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/08 20:49:13 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/24 16:06:49 by aderouba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <define.hpp>
#include <fstream>
#include <unistd.h>

static std::vector<std::string> split(std::string const s, char c)
{
	int							i, j;
	std::vector<std::string>	res;

	i = 0;
	while (s[i] != '\0')
	{
		while (s[i] == c)
			i++;
		if (s[i] == '\0')
			break ;
		j = 0;
		while (s[i + j] != c && s[i + j] != '\0')
			j++;
		res.push_back(s.substr(i, j));
		i += j;
	}
	return (res);
}


std::vector<Vec3> parse(char *name)
{
	std::string file_name = std::string(name);
	std::ifstream file;
	std::vector<Vec3> point_list;
	Vec3	offset;
	if (file_name.size() <= 5 || file_name.substr(file_name.size() - 5) != std::string(".mod1") || file_name[file_name.size() - 6] == '/')
		throw std::invalid_argument("Error : File extension incorrrect.");

	offset = Vec3(MAX_HEIGHT, MAX_HEIGHT, 0.0);

	file.open(file_name);
	if (file.is_open())
	{
		while (file)
		{
			std::string new_line;
			std::getline(file, new_line);
			std::vector<std::string> string_list = split(new_line, ' ');
			if (string_list.size() == 0)//empty line check
				continue;
			if (string_list[0][0] == '#')//comment line
				continue;
			if (string_list.size() != 3)// check number of coordonate for point
				throw std::invalid_argument("Error : invalid number of argument for a point.");
			for (std::string str : string_list)
			{
				int	size = str.size();
				for (int i = 0; i < size; i++)
				{
					if (str[i] != '0')
					{
						if (!isdigit(str[i]))
							throw std::invalid_argument("Error : point cordonate need to pe represented as digit.");
						if (str.size() - i > 2)
							throw std::invalid_argument("Error : Number for point coordonate can't be upper than 99.");
					}

				}

			}
			Vec3 vec = Vec3(atoi(string_list[0].c_str()), atoi(string_list[1].c_str()), atoi(string_list[2].c_str()));
			if (vec.x > MAX_XY)
				throw std::invalid_argument("Error : X coordonate for point can't be higher than " + std::to_string(MAX_XY));
			if (vec.y > MAX_XY)
				throw std::invalid_argument("Error : Y coordonate for point can't be higher than " + std::to_string(MAX_XY));
			if (vec.z > MAX_HEIGHT)
				throw std::invalid_argument("Error : Z coordonate for point can't be upper than " + std::to_string(MAX_HEIGHT));
			vec += offset;
			point_list.push_back(vec);
		}
	}
	else
	{
		file.close();
		throw std::runtime_error("Error : File can't be opened.");
	}
	file.close();
	return (point_list);
}


std::vector<std::vector<double>> interpolate(std::vector<Vec3> & parameterPoints)
{
	double	px, py, pz, dx, dy, dist, val;
	std::vector<double> possible_heights;
	std::vector<std::vector<double>> heightmap;
	char spin[5] = "-\\|/";
	int	ind = 0;

	for (int y = 0; y < MAP_SIZE; y++)
	{
		std::vector<double> line;
		for (int x = 0; x < MAP_SIZE; x++)
			line.push_back(0.0);
		heightmap.push_back(line);
	}
	int	percentNb = (MAP_SIZE * MAP_SIZE) / 100;
	int	percent = 0;
	int	compter = 0;
	std::cout << "Creating Map :" << std::endl;
	printf("    0%%\n");
	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			compter++;
			if (compter >= percentNb)
			{
				compter = 0;
				percent++;
				printf("\e[1A\e[K%c %3i%%\n", spin[ind], percent);
				ind++;
				if (ind == 4)
					ind = 0;
			}
			possible_heights.clear();
			for (Vec3 & point: parameterPoints)
			{
				px = point.x;
				py = point.y;
				pz = point.z;

				dx = px - x;
				dy = py - y;
				dist = (dx * dx) + (dy * dy);
				if (dist == 0)
				{
					possible_heights.push_back(pz);
					continue;
				}

				dist = std::sqrt(dist);
				val = std::max(0.0, pz*pz - dist*dist);
				if (val == 0)
					continue;

				val /= pz*pz;
				val = val*val*val;
				val = val * pz;
				if (val <= TERRAIN_PRECISION)
					continue;

				possible_heights.push_back(val);

			}
			if (possible_heights.size() == 0)
					continue;

			val = 0.0;
			for (double testVal : possible_heights)
			{
				if (testVal > val)
					val = testVal;
			}
			heightmap[y][x] = val;
		}
	}
	printf("\e[1A\e[K100%%  \n");
	return (heightmap);
}
