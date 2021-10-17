#pragma once

#include <gl/glew.h>
#include <glm/mat4x4.hpp>

enum shader_type
{
	vertex_col,
	specular_diffuse
};

inline GLuint get_uniform_count(shader_type type)
{
	switch (type)
	{
	case vertex_col:
		return 1;
	case specular_diffuse:
		return -1;
	default:
		return -1;
	}
}

