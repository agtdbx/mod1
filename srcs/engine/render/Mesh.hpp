#ifndef MESH_HPP
# define MESH_HPP

# include <vector>

# include <model/Point.hpp>
# include <engine/render/Shader.hpp>
# include <engine/render/TextureManager.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	void	translate(glm::vec3 translation);
	void	rotate(glm::vec3 rotation, float degrees);
	void	scale(glm::vec3 scale);

	void	draw(Shader *shader);
	void	drawWithTexture(Shader *shader, TextureManager *textureManager, std::string textureName);
	void	print(void);

private:
	float			*vertices;
	unsigned int	*indices;
	int				nbVertices, nbIndices;
	glm::mat4		model;
};

#endif
