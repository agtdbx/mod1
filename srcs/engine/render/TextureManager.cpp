#include <engine/render/TextureManager.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

TextureManager::TextureManager(void)
{

}


TextureManager::TextureManager(const TextureManager &obj)
{
	this->textures = obj.textures;
}

//---- Destructor --------------------------------------------------------------

TextureManager::~TextureManager()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

TextureManager	&TextureManager::operator=(const TextureManager &obj)
{
	if (this == &obj)
		return (*this);

	this->textures = obj.textures;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************


void	TextureManager::addTexture(std::string textureName, std::string texturePath)
{
	int width, height, nrChannels;
	unsigned char *data;
	unsigned int texture;

	data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
		throw std::invalid_argument("Texture '" + texturePath + "' not found");

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	this->textures.insert(std::pair<std::string, unsigned int>(textureName, texture));
}


unsigned int	TextureManager::getTexture(std::string textureName)
{
	return (this->textures.at(textureName));
}

//**** PRIVATE METHODS *********************************************************
