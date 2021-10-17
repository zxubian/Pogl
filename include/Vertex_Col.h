#pragma once


void get_uniforms_vertex_col(GLuint id, GLint* uniforms);

void bind_uniforms_vertex_col(
	const GLint* uniforms,
	const glm::mat4x4& model_matrix,
	const glm::mat4x4& vp_matrix
);

void bind_inputs_vertex_col()
