#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>

struct Directional_light
{
	glm::vec3 direction{};

	/// <summary>
	/// color	intensity
	/// rgb		a
	/// </summary>
	glm::vec4 color{};

public:
	Directional_light(glm::vec3 direction, glm::vec4 color)
	{
		this->direction = direction;
		this->color = color;
	}
	Directional_light()
	{
		direction = glm::vec3(0, -1, 0);
		color = glm::vec4(1,1,1,1);
	}

	~Directional_light() = default;
};

struct LightData
{
	/// <summary>
	/// color	intensity
	/// rgb		a
	/// </summary>
	glm::vec4 ambient_light;

	Directional_light directional_light;
};
