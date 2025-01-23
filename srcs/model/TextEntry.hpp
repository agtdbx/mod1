/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextEntry.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <lflandri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 21:51:07 by lflandri          #+#    #+#             */
/*   Updated: 2025/01/14 22:08:47 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXTENTRY_HPP
# define TEXTENTRY_HPP

# include <define.hpp>
# include <engine/render/Mesh.hpp>
# include <engine/inputs/Mouse.hpp>
# include <engine/inputs/InputManager.hpp>
# include <engine/OpenGLContext.hpp>
# include <engine/render/shader/ShaderManager.hpp>
# include <engine/render/TextureManager.hpp>
# include <model/Point2D.hpp>

# define BUTTON_BASE_COLOR_TYPE 0
# define BUTTON_UNDERLINE_COLOR_TYPE 1

class TextEntry
{
public:

	static Mouse * mouse;
	static InputManager * inputManager;
	static TextureManager * textureManager;

	TextEntry(float x, float y, float width, float height);
	TextEntry(float x, float y, float width, float height, glm::vec3 baseColor, glm::vec3 underlineColor);
	TextEntry(const TextEntry &obj);
	~TextEntry();

	TextEntry	&operator=(const TextEntry &obj);

	void		renderMesh( ShaderManager *shaderManager);
	glm::vec2	getPos();
	std::string	getValue();
	void	setPos(float x, float y);
	void	setColor(glm::vec3, int type);


	

private:
	float		x_screen, y_screen, width, height;
	bool 		isSelected;
	std::string	value;
	glm::vec3		baseColor, underlineColor;

	bool			mouseOnTextEntry();
	void			addCharToValue(char c);
	void			removeLastCharFromValue();


};

#endif
