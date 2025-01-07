#ifndef MESH_HPP
# define MESH_HPP

# include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

# include <model/Point.hpp>
# include <engine/render/shader/Shader.hpp>
# include <engine/render/TextureManager.hpp>
# include <engine/render/Camera.hpp>

typedef struct s_tri_id
{
	unsigned int	p1;
	unsigned int	p2;
	unsigned int	p3;
}	t_tri_id;


class Mesh
{
public:
	Mesh(void);
	Mesh(std::vector<Point> &vertices, std::vector<t_tri_id> &indices);
	Mesh(const Mesh &obj);
	~Mesh();

	Mesh	&operator=(const Mesh &obj);

	void	setVertices(std::vector<Point> &vertices);
	void	setMesh(std::vector<Point> &vertices, std::vector<t_tri_id> &indices);

	void	translate(glm::vec3 translation);
	void	rotate(glm::vec3 rotation, float degrees);
	void	scale(float scale);

	void	draw(Camera *camera, Shader *shader, unsigned int VAOid);
	void	print(void);

private:
	float			*vertices, degrees_x, degrees_y, degrees_z, scaleFactor;
	unsigned int	*indices;
	int				nbVertices, nbIndices;
	glm::vec3		translation;
	glm::mat4		model;

	void	computeModelMatrix(void);
};

#endif
