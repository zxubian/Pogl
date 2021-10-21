#include "../include/Camera.h"

#include <glm/ext/matrix_transform.hpp>

Camera::Camera(const glm::vec3 position, const glm::vec3 forward, const glm::vec3 up)
{
	this->position = position;
	pitch = asin(forward.y);
	const GLfloat cos_pitch = cos(pitch);
	yaw = acos(forward.x / cos_pitch);
	this->forward = forward;
	this->up = up;
	this->right = glm::normalize(glm::cross(up, forward));
	view = glm::lookAt(position, forward, up);
	transform = glm::mat4x4
	(
		right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		forward.x, forward.y, forward.z, 0,
		position.x, position.y, position.z, 1
	);
}

Camera::Camera(const glm::vec3 position, const GLfloat pitch, const GLfloat yaw)
{
	this->position = glm::vec4(position.x, position.y, position.z, 1);
	this->pitch = pitch;
	this->yaw = yaw;
	recalculate_matricies();
}

void Camera::recalculate_matricies()
{
	forward = glm::vec3{ cos(pitch) * cos(yaw), sin(pitch), cos(pitch) * sin(yaw) };
	right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
	up = glm::normalize(glm::cross(right, forward));
	view = glm::lookAt(position, position + forward, up);
	transform = glm::mat4x4
	(
		right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		forward.x, forward.y, forward.z, 0,
		position.x, position.y, position.z, 1
	);
}

Camera::~Camera()
= default;
