#include <engine/render/shader/ShaderFunctions.hpp>

//**** FUNCTIONS ***************************************************************

void	giveIntToShader(int shaderId, const char *variableName, int value)
{
	int variableLoc = glGetUniformLocation(shaderId, variableName);
	glUniform1i(variableLoc, value);
}


void	giveFloatToShader(int shaderId, const char *variableName, float value)
{
	int variableLoc = glGetUniformLocation(shaderId, variableName);
	glUniform1f(variableLoc, value);
}


void	giveVec3ToShader(int shaderId, const char *variableName, glm::vec3 value)
{
	int variableLoc = glGetUniformLocation(shaderId, variableName);
	glUniform3fv(variableLoc, 1, glm::value_ptr(value));
}


void	giveFloatTextureToShader(int shaderId, const char *variableName, int textureId,
									GLuint textureBuffer, GLuint texture)
{
	glActiveTexture(GL_TEXTURE0 + textureId);
	glBindTexture(GL_TEXTURE_BUFFER, texture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, textureBuffer);
	glUniform1i(glGetUniformLocation(shaderId, variableName), textureId);
}


void	giveVec3TextureToShader(int shaderId, const char *variableName, int textureId,
									GLuint textureBuffer, GLuint texture)
{
	glActiveTexture(GL_TEXTURE0 + textureId);
	glBindTexture(GL_TEXTURE_BUFFER, texture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, textureBuffer);
	glUniform1i(glGetUniformLocation(shaderId, variableName), textureId);
}


void	giveVec4TextureToShader(int shaderId, const char *variableName, int textureId,
									GLuint textureBuffer, GLuint texture)
{
	glActiveTexture(GL_TEXTURE0 + textureId);
	glBindTexture(GL_TEXTURE_BUFFER, texture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, textureBuffer);
	glUniform1i(glGetUniformLocation(shaderId, variableName), textureId);
}
