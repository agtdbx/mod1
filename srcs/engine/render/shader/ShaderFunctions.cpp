#include <engine/render/shader/ShaderFunctions.hpp>

//**** FUNCTIONS ***************************************************************

void	giveBoolToShader(int shaderId, const char *variableName, bool value)
{
	int variableLoc = glGetUniformLocation(shaderId, variableName);
	glUniform1i(variableLoc, value);
}


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


void	giveVec2ToShader(int shaderId, const char *variableName, glm::vec2 value)
{
	int variableLoc = glGetUniformLocation(shaderId, variableName);
	glUniform2fv(variableLoc, 1, glm::value_ptr(value));
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


void	giveFloatTextureInputToShader(int textureId, bool readable,
										GLuint textureBuffer, GLuint texture)
{
	glActiveTexture(GL_TEXTURE0 + textureId);
	glBindTexture(GL_TEXTURE_BUFFER, texture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, textureBuffer);
	if (readable)
		glBindImageTexture(textureId, texture, 0, GL_FALSE, 0,
							GL_READ_WRITE, GL_R32F);
	else
		glBindImageTexture(textureId, texture, 0, GL_FALSE, 0,
							GL_WRITE_ONLY, GL_R32F);
}


void	giveVec4TextureInputToShader(int textureId, bool readable,
										GLuint textureBuffer, GLuint texture)
{
	glActiveTexture(GL_TEXTURE0 + textureId);
	glBindTexture(GL_TEXTURE_BUFFER, texture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, textureBuffer);
	if (readable)
		glBindImageTexture(textureId, texture, 0, GL_FALSE, 0,
							GL_READ_WRITE, GL_RGBA32F);
	else
		glBindImageTexture(textureId, texture, 0, GL_FALSE, 0,
							GL_WRITE_ONLY, GL_RGBA32F);
}

void	giveSSBBOInputToShader(int bufferId, GLuint ssbo)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bufferId, ssbo);
}
