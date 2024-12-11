#ifndef CAMERA_HPP
# define CAMERA_HPP

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include <define.hpp>

class Camera
{
public:
	Camera(void);
	Camera(const Camera &obj);
	~Camera();

	glm::mat4	getView(void);
	glm::mat4	getProjection(void);

	Camera	&operator=(const Camera &obj);

private:
	glm::mat4	view, projection;
};

#endif
