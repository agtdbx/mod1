#ifndef WATERSIMULATION_HPP
# define WATERSIMULATION_HPP

# include <vector>
# include <unordered_map>

# include <engine/render/Mesh.hpp>
# include <engine/render/Camera.hpp>
# include <engine/render/ShaderManager.hpp>
# include <engine/maths/Vec3.hpp>

class WaterSimulation
{
public:
	WaterSimulation(void);
	WaterSimulation(const WaterSimulation &obj);
	~WaterSimulation();

	int	getNbParticules(void);

	WaterSimulation	&operator=(const WaterSimulation &obj);

	void	addWater(glm::vec3 position);
	void	tick(float delta);
	void	draw(Camera *camera, ShaderManager *shaderManager);

private:
	std::vector<glm::vec3>	positions;
	std::vector<glm::vec3>	velocities;
	std::vector<double>		densities;
	float					triangleOverScreen[12];
	int						nbParticules;
	GLuint					textureBuffer, texture;

	void		generateTextureBuffer(void);
	void		generateTriangleOverScreen(void);
};

#endif
