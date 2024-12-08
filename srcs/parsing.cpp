/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <liam.flandrinck.58@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/08 20:49:13 by lflandri          #+#    #+#             */
/*   Updated: 2024/12/09 00:40:44 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <define.hpp>
#include <fstream>

static std::vector<std::string>    split(std::string const s, char c)
{
    int                            i, j;
    std::vector<std::string>    res;

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
	if (file_name.size() <= 5 || file_name.substr(file_name.size() - 5) != std::string(".mod1") || file_name[file_name.size() - 6] == '/')
		throw std::invalid_argument("Error : File extension incorrrect.");
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
				for ( int i = 0; i < str.size(); i++)
				{
					if (str[i] != '0')
					{
						if (!isdigit(str[i]))
							throw std::invalid_argument("Error : point cordonate need to pe represented as digit.");
						if (str.size() - i > 4)
							throw std::invalid_argument("Error : Number for point coordonate can't be upper than 9999.");
					}
					
				}

			}
			Vec3 vec = Vec3(atoi(string_list[0].c_str()), atoi(string_list[1].c_str()), atoi(string_list[2].c_str()));
			point_list.push_back(vec);
		}
	}
	else
	{
		file.close();
		throw std::runtime_error("Error : File can't be opened.");
	}
	file.close();
	return point_list;
}
