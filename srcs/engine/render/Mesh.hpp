#ifndef MESH_HPP
# define MESH_HPP

# include <vector>

# include <engine/maths/Vec3.hpp>
# include <engine/render/Shader.hpp>

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
	Mesh(std::vector<Vec3> &vertices, std::vector<t_tri_id> &indices);
	Mesh(const Mesh &obj);
	~Mesh();

	Mesh	&operator=(const Mesh &obj);

	void	draw(Shader *shader);
	void	print(void);

private:
	float			*vertices;
	unsigned int	*indices;
	int				nbVertices, nbIndices;
};

#endif
