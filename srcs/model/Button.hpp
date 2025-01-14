/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Button.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 21:51:07 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/14 22:08:47 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUTTON_HPP
# define BUTTON_HPP

# include <define.hpp>
# include <engine/render/Mesh.hpp>
# include <engine/inputs/Mouse.hpp>
# include <engine/OpenGLContext.hpp>
# include <engine/render/shader/ShaderManager.hpp>
# include <engine/render/TextureManager.hpp>
# include <model/Point2D.hpp>

# define BUTTON_BASE_COLOR_TYPE 0
# define BUTTON_UNDERLINE_COLOR_TYPE 1

class Button
{
public:

	static Mouse * mouse;

	Button(float x, float y, float width, float height, void (*functionToExecute)(void *), void *arg, unsigned int texture);
	Button(float x, float y, float width, float height, void (*functionToExecute)(void *), void *arg, unsigned int texture, glm::vec3 baseColor, glm::vec3 underlineColor);
	Button(const Button &obj);
	~Button();

	Button	&operator=(const Button &obj);

	void		renderMesh( ShaderManager *shaderManager);
	glm::vec2	getPos();
	void		active();
	void		desactive();
	bool	isActive();
	void	press();
	void	setArg(void *arg);
	void	setSwitchMode(bool isSwitch);
	void	setPos(float x, float y);
	void	setColor(glm::vec3, int type);

	

private:
	float		x_screen, y_screen, width, height;
	unsigned int	texture;
	glm::vec3		baseColor, underlineColor;
	void			(*functionToExecute)(void *);
	void			*arg;
	bool			active_B, isSwitch, isON;

	bool			mouseOnButton();


};

#endif
