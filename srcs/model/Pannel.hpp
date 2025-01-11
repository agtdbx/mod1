/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pannel.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <liam.flandrinck.58@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 21:50:55 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/11 04:14:26 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PANNEL_HPP
# define PANNEL_HPP

# include <define.hpp>
# include <engine/render/Mesh.hpp>
# include <engine/inputs/Mouse.hpp>
# include <engine/OpenGLContext.hpp>
# include <engine/render/shader/ShaderManager.hpp>
# include <engine/render/TextureManager.hpp>
# include <model/Point2D.hpp>
# include <model/Button.hpp>

class Pannel
{
public:

	static Mouse * mouse;

	Pannel(float x, float y, float width, float height, unsigned int texture);
	Pannel(float x, float y, float width, float height, unsigned int texture, glm::vec3 baseColor);
	Pannel(const Pannel &obj);
	~Pannel();

	Pannel	&operator=(const Pannel &obj);
	Button& operator[](int);
	void	setPos(float x, float y);
	void	setPosToGo(float x, float y);

	void	renderMesh( ShaderManager *shaderManager);
	void	addButton(Button  b);
	void	tick(double delta);
	

private:
	float		x_screen, y_screen, width, height, padding, x_toGo, y_toGo;
	glm::vec3		baseColor;
	unsigned int	texture;
	std::vector<Button>	buttonList;

	void	moveButton(float mvx, float mvy);



};

#endif
