#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class Camera
{

public:
	Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 up);
	static glm::mat4x4 calculate_view_matrix(glm::mat4x4 transform);
	const glm::mat4x4& transform_matrix = transform;
	const glm::mat4x4& view_matrix = view;
	void Camera::set_transform(glm::mat4x4 transform);
	void set_position(glm::vec3 position)
	{
		transform[3] = glm::vec4(position.x, position.y, position.z, 1);
	}
	void set_rotation(glm::quat rotation);
	glm::vec4 get_position() { return transform[3]; }
	glm::quat get_rotation() { return rotation; }
	~Camera();
private:
	glm::vec3 forward;
	glm::quat rotation;
	glm::mat4x4 transform{};
	glm::mat4x4 view{};
};
