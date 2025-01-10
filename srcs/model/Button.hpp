#ifndef BUTTON_HPP
# define BUTTON_HPP

# include <define.hpp>
# include <engine/render/Mesh.hpp>
# include <engine/inputs/Mouse.hpp>
# include <engine/OpenGLContext.hpp>
# include <engine/render/shader/ShaderManager.hpp>
# include <engine/render/TextureManager.hpp>
# include <model/Point2D.hpp>

class Button
{
public:

	static Mouse * mouse;

	Button(float x, float y, float width, float height, void (*functionToExecute)(void *), void *arg, unsigned int texture);
	Button(float x, float y, float width, float height, void (*functionToExecute)(void *), void *arg, unsigned int texture, glm::vec3 baseColor, glm::vec3 underlineColor);
	Button(const Button &obj);
	~Button();

	Button	&operator=(const Button &obj);

	void	renderMesh( ShaderManager *shaderManager);
	void	active();
	void	desactive();
	bool	isActive();
	void	press();
	void	setArg(void *arg);
	

private:
	const float		x_screen, y_screen, width, height;
	unsigned int	texture;
	glm::vec3		baseColor, underlineColor;
	void			(*functionToExecute)(void *);
	void			*arg;
	bool			active_B;

	bool			mouseOnButton();


};

#endif
