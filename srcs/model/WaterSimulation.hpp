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

	WaterSimulation	&operator=(const WaterSimulation &obj);

	void	addWater(Vec3 position);
	void	tick(double delta);
	void	draw(Camera *camera, ShaderManager *shaderManager);

private:
	Mesh				mesh;
	std::vector<Vec3>	positions;
	std::vector<Vec3>	velocities;
	int					nbParticules;
	static const char	triangleListPoint[256][16];

	void	createMesh(void);
};

#endif
