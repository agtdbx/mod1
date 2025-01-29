/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pannel.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gugus <gugus@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 21:50:55 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/29 19:49:51 by gugus            ###   ########.fr       */
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
# include <ui/Button.hpp>
# include <ui/Slider.hpp>
# include <ui/TextEntry.hpp>

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
	Slider& operator[](float);
	TextEntry& operator[](char);
	void	setPos(float x, float y);
	void	setPosToGo(float x, float y);
	void	addPosToGo(float x, float y);

	void	renderMesh( ShaderManager *shaderManager);
	void	addButton(Button  b);
	void	addSlider(Slider  s);
	void	addTextEntry(TextEntry  t);
	void	tick(double delta);

private:
	float		x_screen, y_screen, width, height, padding, x_toGo, y_toGo;
	glm::vec3		baseColor;
	unsigned int	texture;
	std::vector<Button>	buttonList;
	std::vector<Slider>	sliderList;
	std::vector<TextEntry>	textEntryList;

	void	moveButton(float mvx, float mvy);
};

#endif
