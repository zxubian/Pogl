#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class Camera
{

public:
	Camera(glm::mat4x4 transform);
	static glm::mat4x4 calculate_view_matrix(glm::mat4x4 transform);
	const glm::mat4x4& transform_matrix = transform;
	const glm::mat4x4& view_matrix = view;
	void set_transform(glm::mat4x4 transform);
	~Camera();
private:
	glm::mat4x4 transform{};
	glm::mat4x4 view{};
};
