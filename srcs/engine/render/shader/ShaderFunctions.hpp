#ifndef SHADERFUNCTIONS_HPP
# define SHADERFUNCTIONS_HPP

# include <GL/glew.h>
# include <GLFW/glfw3.h>
# include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

void	giveBoolToShader(int shaderId, const char *variableName, bool value);
void	giveIntToShader(int shaderId, const char *variableName, int value);
void	giveFloatToShader(int shaderId, const char *variableName, float value);
void	giveVec2ToShader(int shaderId, const char *variableName, glm::vec2 value);
void	giveVec3ToShader(int shaderId, const char *variableName, glm::vec3 value);
void	giveFloatTextureToShader(int shaderId, const char *variableName, int textureId,
									GLuint textureBuffer, GLuint texture);
void	giveVec3TextureToShader(int shaderId, const char *variableName, int textureId,
									GLuint textureBuffer, GLuint texture);
void	giveVec4TextureToShader(int shaderId, const char *variableName, int textureId,
									GLuint textureBuffer, GLuint texture);
void	giveFloatTextureInputToShader(int textureId, bool readable,
										GLuint textureBuffer, GLuint texture);
void	giveVec4TextureInputToShader(int textureId, bool readable,
										GLuint textureBuffer, GLuint texture);
void	giveSSBBOInputToShader(int bufferId, GLuint ssbo);

#endif
