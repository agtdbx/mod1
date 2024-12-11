#ifndef OPENGLCONTEXT_HPP
# define OPENGLCONTEXT_HPP

# include <define.hpp>

class OpenGLContext
{
public:
	GLFWwindow	*window;

	OpenGLContext(void);
	OpenGLContext(const OpenGLContext &obj);
	~OpenGLContext();

	OpenGLContext	&operator=(const OpenGLContext &obj);

	void	setDrawMode(bool fillTriangle);

private:
	bool	initGood;
	int		winW, winH;

	void	quitWhenError(std::string errorMessage);
};

#endif
