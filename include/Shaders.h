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

void get_uniforms_vertex_col(GLuint id, GLint* uniforms);

void bind_uniforms_vertex_col(
	const GLint* uniforms,
	const glm::mat4x4& model_matrix,
	const glm::mat4x4& vp_matrix
);

void bind_inputs_vertex_col()
{

};
