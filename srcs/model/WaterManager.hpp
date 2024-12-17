/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WaterManager.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aderouba <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 15:25:16 by lflandri          #+#    #+#             */
/*   Updated: 2024/12/17 12:37:32 by aderouba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WATERMANAGER
# define WATERMANAGER
# include <model/Water.hpp>
# include <engine/render/ShaderManager.hpp>
# include <engine/maths/Vec3.hpp>


class WaterManager
{
private:

	std::vector<Water> waterDrops;
	std::vector<std::vector<std::vector<bool> > > waterMapPoint;
	std::vector<Vec3> pointSave;

protected:

	/* data */

public:

	static const int triangleListPoint[256][16];

	WaterManager(void);					//constructor
	~WaterManager();							//destructor
	WaterManager (const WaterManager &obj);				//copie constructor
	WaterManager& operator=(const WaterManager& x);		//assignation constructor

	std::vector<Water> & getWaterDrops();
	std::vector<std::vector<std::vector<bool> > > & getWaterMapPoint();
	void	addWaterDrop(Water drop);

	void setWaterDrops(std::vector<Water>);

	void updatePosition();
	void updateDensity();
	void draw(Camera *camera, ShaderManager *shaderManager);
	void calculWaterInGrid();

};
#endif
