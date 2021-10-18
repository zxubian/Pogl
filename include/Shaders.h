#pragma once

#include <gl/glew.h>
#include <glm/mat4x4.hpp>

enum shader_type
{
	vertex_col,
	specular_diffuse
};

inline void get_uniforms_vertex_col(GLuint id, GLint* uniforms)
{
	uniforms[0] = glGetUniformLocation(id, "vp_matrix");
	uniforms[1] = glGetUniformLocation(id, "model_matrix");
}

constexpr GLuint get_uniform_count(const shader_type type)
{
	switch (type)
	{
	case vertex_col:
		return 2;
	case specular_diffuse:
		return -1;
	default:
		return -1;
	}
}

