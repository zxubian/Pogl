#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Transform.h"
#include "Light.h"

class Texture;

struct Program_render_data
{
	GLuint vertex_col_shader_id;
	GLuint specular_diffuse_shader_id;
};

struct Specular_diffuse_material_data
{
	glm::vec4 diffuse_color;
	glm::vec3 specular_color;
	glm::vec3 ambient_color;
	glm::vec4 intensities;
	Texture* albedo_texture;
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

struct Specular_diffuse_data
{
	Mesh_render_data* meshes;
	Specular_diffuse_material_data** material_data;
	unsigned int distinct_mesh_count;
};

struct Things_to_render
{
	Vertex_col_data vertex_col_data;
	Specular_diffuse_data specular_diffuse_data;
};

struct Per_frame_render_data
{
	glm::mat4x4 projection_matrix;
	glm::mat4x4 view_matrix;

	Light_data light_data;

	Per_frame_render_data(const glm::mat4x4& projection_matrix, const glm::mat4x4& view_matrix,
		const Light_data& light_data)
		: projection_matrix(projection_matrix),
		view_matrix(view_matrix),
		light_data(light_data)
	{ }
};

void render(const Program_render_data* program_render_data, const Per_frame_render_data* per_frame_render_data, const Things_to_render* things_to_render);
