#include "../include/Camera.h"

Camera::Camera(const glm::vec3 position, const glm::vec3 forward, const glm::vec3 up)
{
	this->forward = forward;
	rotation = glm::quatLookAt(forward, up);
	transform = glm::mat4x4{ 1 };
	transform = glm::translate(transform, position);
	transform *= glm::mat4_cast(rotation);
	set_transform(transform);
}

inline glm::mat4x4 Camera::calculate_view_matrix(const glm::mat4x4 transform)
{
	return glm::inverse(transform);
}

void Camera::set_transform(const glm::mat4x4 transform)
{
	this->transform = transform;
	view = calculate_view_matrix(transform);
}

void Camera::set_rotation(const glm::quat rotation)
{
	this->rotation = rotation;
	glm::mat4x4 rotation_matrix = glm::mat4_cast(rotation);
	rotation_matrix[3] = transform[3];
	set_transform(rotation_matrix);
}

Camera::~Camera()
= default;
