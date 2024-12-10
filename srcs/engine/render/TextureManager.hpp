#ifndef TEXTUREMANAGER_HPP
# define TEXTUREMANAGER_HPP

# include <iostream>
# include <map>

class TextureManager
{
public:
	TextureManager(void);
	TextureManager(const TextureManager &obj);
	~TextureManager();

	TextureManager	&operator=(const TextureManager &obj);

	void			addTexture(std::string textureName, std::string texturePath);
	unsigned int	getTexture(std::string textureName);

private:
	std::map<std::string, unsigned int>	textures;
};

#endif
