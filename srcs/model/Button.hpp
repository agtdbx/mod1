#ifndef BUTTON_HPP
# define BUTTON_HPP

# include <define.hpp>
# include <engine/render/Mesh.hpp>
# include <engine/render/Camera.hpp>
# include <engine/OpenGLContext.hpp>
# include <engine/render/ShaderManager.hpp>
# include <model/Point2D.hpp>

class Button
{
public:
	Button(float x, float y, float width, float height);
	Button(const Button &obj);
	~Button();

	Button	&operator=(const Button &obj);

	void	renderMesh( ShaderManager *shaderManager);

private:
	const float	x_screen, y_screen, width, height;
	static  Camera camera;
	
	// void	updateMesh(OpenGLContext &);


};

#endif
