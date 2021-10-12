#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class Camera
{

public:
	Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 up);
	Camera(glm::vec3 position, GLfloat pitch, GLfloat yaw);
	const glm::mat4x4& view_matrix = view;
	const glm::mat4x4& transform_matrix = transform;
	glm::vec3 position, forward, right, up;
	GLfloat pitch, yaw;
	void recalculate_matricies();
	~Camera();
private:
	glm::mat4x4 view{1};
	glm::mat4x4 transform{1};
};
