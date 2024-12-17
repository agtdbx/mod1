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

	void	addWater(Vec3 position);
	void	tick(double delta);
	void	draw(Camera *camera, ShaderManager *shaderManager);

private:
	Mesh				mesh;
	std::vector<Vec3>	positions;
	std::vector<Vec3>	velocities;
	std::vector<double>	densities;
	int					nbParticules;
	static const char	trianglePoints[256][16];

	void	createMesh(void);
};

#endif
