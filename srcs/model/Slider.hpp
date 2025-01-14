/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Slider.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 21:51:07 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/14 13:14:18 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SLIDER_HPP
# define SLIDER_HPP

# include <define.hpp>
# include <engine/render/Mesh.hpp>
# include <engine/inputs/Mouse.hpp>
# include <engine/OpenGLContext.hpp>
# include <engine/render/shader/ShaderManager.hpp>
# include <engine/render/TextureManager.hpp>
# include <model/Point2D.hpp>

class Slider
{
public:

	static Mouse * mouse;

	Slider(float x, float y, float width, float height);
	Slider(float x, float y, float width, float height, glm::vec3 baseColor, glm::vec3 underlineColor);
	Slider(const Slider &obj);
	~Slider();

	Slider	&operator=(const Slider &obj);

	void		renderMesh( ShaderManager *shaderManager);
	glm::vec2	getPos();
	void	setValue(float value);
	float	getValue();
	void	setPos(float x, float y);

	static unsigned int	texture;
	

private:
	float		x_screen, y_screen, width, height, value;
	glm::vec3		baseColor, underlineColor;
	bool			hasBeenClick;

	bool			mouseOnSlider();


};

#endif
