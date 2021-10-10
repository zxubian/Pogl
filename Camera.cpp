#include "Camera.h"

Camera::Camera(glm::mat4x4 transform)
{
	set_transform(transform);
}

inline glm::mat4x4 Camera::calculate_view_matrix(glm::mat4x4 transform)
{
	return glm::inverse(transform);
}

void Camera::set_transform(glm::mat4x4 transform)
{
	this->transform = transform;
	view = calculate_view_matrix(transform);
}

Camera::~Camera()
= default;
