#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>

struct Directional_light
{
	glm::vec3 direction;
	/// <summary>
	/// rgb = color, a = intensity
	/// </summary>
	glm::vec4 color;
};

struct Point_light
{
	glm::vec3 world_position;
	glm::vec3 color;
	GLfloat max_distance;
	/// <summary>
	/// Suggested values: 2, 3, etc.
	/// </summary>
	GLfloat falloff;
};

struct Light_data
{
	Directional_light directional_light;
	Point_light* point_lights;
	GLuint point_light_count;
};

inline glm::vec3 calculate_attenuation_constant(GLfloat r_max, GLfloat k)
{
	const GLfloat k2 = k * k;
	const GLfloat exp_k2 = glm::exp(k2);
	const GLfloat one_minus_exp_k2 = 1 - exp_k2;
	const GLfloat r_max2 = r_max * r_max;
	const GLfloat one_over_one_minus_exp_k2 = 1 / (one_minus_exp_k2);
	return glm::vec3(-k2 / r_max2, one_over_one_minus_exp_k2, exp_k2 * one_over_one_minus_exp_k2);
}
