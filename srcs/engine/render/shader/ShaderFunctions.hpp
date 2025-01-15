#ifndef SHADERFUNCTIONS_HPP
# define SHADERFUNCTIONS_HPP

# include <GL/glew.h>
# include <GLFW/glfw3.h>
# include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

void	giveIntToShader(int shaderId, const char *variableName, int value);
void	giveFloatToShader(int shaderId, const char *variableName, float value);
void	giveVec3ToShader(int shaderId, const char *variableName, glm::vec3 value);
void	giveFloatTextureToShader(int shaderId, const char *variableName, int textureId,
									GLuint textureBuffer, GLuint texture);
void	giveVec3TextureToShader(int shaderId, const char *variableName, int textureId,
									GLuint textureBuffer, GLuint texture);
void	giveVec4TextureToShader(int shaderId, const char *variableName, int textureId,
									GLuint textureBuffer, GLuint texture);

#endif
