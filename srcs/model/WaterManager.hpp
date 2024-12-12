/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WaterManager.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 15:25:16 by lflandri          #+#    #+#             */
/*   Updated: 2024/12/12 16:59:10 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WATERMANAGER
# define WATERMANAGER
# include <model/Water.hpp>


class WaterManager 
{
private:

	std::vector<Water> waterDrops;

protected:

	/* data */

public:

	WaterManager(void);					//constructor
	~WaterManager();							//destructor
	WaterManager (const WaterManager &obj);				//copie constructor
	WaterManager& operator=(const WaterManager& x);		//assignation constructor

	std::vector<Water> & getWaterDrops();
	void	addWaterDrop(Water drop);
	
	void setWaterDrops(std::vector<Water>);

	void updatePosition();
	void updateDensity();
	void draw(Camera *camera, Shader *shader, TextureManager *textureManager, std::string textureName);
 
};
#endif
