#include <engine/OpenGLContext.hpp>

//**** STATIC FUNCTIONS ********************************************************
// Error callback function for GLFW
static void	print_opengl_error(int error, const char* description)
{
	std::cerr << "Opengl error : " << description << std::endl;
}

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

OpenGLContext::OpenGLContext(void)
{
	this->window = NULL;
	this->initGood = false;
	this->winW = WIN_W;
	this->winH = WIN_H;

	// Init opengl
	if (!glfwInit())
	{
		this->quitWhenError("Failed to initialize GLFW");
		return ;
	}

	// Set opengl print error function
	glfwSetErrorCallback(print_opengl_error);

	// Create window
	this->window = glfwCreateWindow(this->winW, this->winH, WIN_TITLE, nullptr, nullptr);
	if (!this->window)
	{
		this->quitWhenError("Failed to create window");
		return ;
	}

	// Setup opengl context
	glfwMakeContextCurrent(this->window);

	// Init GLEW (OpenGL Extension Wrangler)
	if (glewInit() != GLEW_OK)
	{
		this->quitWhenError("Failed to initialize GLEW");
		return ;
	}

	// Set the OpenGL viewport size
	glViewport(0, 0, this->winW, this->winH);
}


OpenGLContext::OpenGLContext(const OpenGLContext &obj)
{
	this->window = NULL;
	this->initGood = false;
}

//---- Destructor --------------------------------------------------------------

OpenGLContext::~OpenGLContext()
{
	if (this->window != NULL)
		glfwDestroyWindow(this->window);

	glfwTerminate();
}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

OpenGLContext	&OpenGLContext::operator=(const OpenGLContext &obj)
{
	if (this == &obj)
		return (*this);

	if (this->window != NULL)
		glfwDestroyWindow(this->window);
	this->window = NULL;

	this->initGood = false;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	OpenGLContext::setDrawMode(bool fillTriangle)
{
	// To change how opengl render triangle (fill / line)
	if (fillTriangle)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

//**** PRIVATE METHODS *********************************************************

void	OpenGLContext::quitWhenError(std::string errorMessage)
{
	std::cerr << "Error : " << errorMessage << std::endl;
}
