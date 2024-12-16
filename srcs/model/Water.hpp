/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Water.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:55:30 by lflandri          #+#    #+#             */
/*   Updated: 2024/12/16 18:22:52 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WATER
# define WATER
# include <define.hpp>
# include "engine/maths/Vec3.hpp"
# include <engine/render/Mesh.hpp>
# include <engine/render/Camera.hpp>
# include <engine/render/Shader.hpp>
# include <engine/render/TextureManager.hpp>
# include <engine/maths/utils.hpp>
class Water 
{
private:
	// std::vector<Point>		vertices;
	Vec3 position;
	Vec3 velocity;
	double gravity;
	double density;
	// Mesh mesh;


protected:

	/* data */

public:

	Water(void);					//constructor
	Water(Vec3 pos, Vec3 vel, double grav);					//constructor
	~Water();							//destructor
	Water (const Water &obj);				//copie constructor
	Water& operator=(const Water& x);		//assignation constructor

	Vec3 getPosition();
	Vec3 getVelocity();
	double getGravity();
	double getDensity();
	// Mesh & getMesh();
 
	void setPosition(Vec3 pos);
	void setVelocity(Vec3 vel);
	void setGravity(double grave);
	// void setMesh(Mesh newMesh);

	// void actualiseMeshPosition(void);
	void update(void);
	void calculSelfDensity(std::vector<Water> waterDrops);
	// void renderMesh(Camera *camera, Shader *shader, TextureManager *textureManager, std::string textureName);

};
#endif
