#ifndef WATERSIMULATION_HPP
# define WATERSIMULATION_HPP

# include <vector>
# include <unordered_map>

# include <engine/render/Mesh.hpp>
# include <engine/render/Camera.hpp>
# include <engine/render/ShaderManager.hpp>
# include <engine/maths/Vec3.hpp>

typedef struct s_water_grid_pos
{
	int	x;
	int	y;
	int	z;
}	t_water_grid_pos;

class WaterSimulation
{
public:
	WaterSimulation(void);
	WaterSimulation(const WaterSimulation &obj);
	~WaterSimulation();

	int	getNbParticules(void);

	WaterSimulation	&operator=(const WaterSimulation &obj);

	void	addWater(glm::vec3 position);
	void	addWaters(std::vector<glm::vec3> positions);
	void	tick(float delta);
	void	draw(Camera *camera, ShaderManager *shaderManager);

private:
	glm::vec3	*positions;
	glm::vec3	*velocities;
	double		*densities;
	int			nbParticules;

	void		freeArrays(void);
};

#endif
