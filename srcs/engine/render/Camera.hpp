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

	void	moveFront(float speed);
	void	moveUp(float speed);
	void	moveRight(float speed);
	void	rotateX(float degrees);
	void	rotateY(float degrees);
	void	rotateZ(float degrees);

private:
	glm::mat4	view, projection;
	glm::vec3	position, front, up, right;
	float		pitch, yaw, roll;

	void		computeRotation(void);
	void		computeView(void);
};

#endif
