#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#include "Mesh.h"
#include "Transform.h"

struct Program_render_data
{
	GLuint vertex_col_shader_id;
	GLint* vertex_col_uniforms;
	GLuint specular_diffuse_shader_id;
	GLint* specular_diffuse_uniforms;
};

struct Per_frame_render_data
{
	glm::mat4x4 projection_matrix;
	glm::mat4x4 view_matrix;

	Per_frame_render_data(const glm::mat4x4& projection_matrix, const glm::mat4x4& view_matrix)
		: projection_matrix(projection_matrix),
		  view_matrix(view_matrix)
	{}
};

struct Mesh_render_data
{
	Mesh* mesh;
	Transform* transforms;
	unsigned int instance_count;
};

struct Vertex_col_data
{
	Mesh_render_data* meshes;
	unsigned int distinct_mesh_count;
};

struct Things_to_render
{
	Vertex_col_data vertex_col_data;
};

void render(const Program_render_data* program_render_data, const Per_frame_render_data* per_frame_render_data, const Things_to_render* things_to_render);
