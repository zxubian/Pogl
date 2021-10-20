#pragma once

#include <gl/glew.h>
#include <glm/mat4x4.hpp>

enum class shader_type
{
	vertex_col,
	specular_diffuse
};

constexpr GLuint get_uniform_count(const shader_type type)
{
	switch (type)
	{
	case shader_type::vertex_col:
		return 2;
	case shader_type::specular_diffuse:
		return 11;
	default:
		return -1;
	}
}

static constexpr char* const vertex_col_uniform_names[] =
{
	"vp_matrix",
	"model_matrix"
};

static constexpr char* const specular_diffuse_uniform_names[] =
{
	"model_matrix",
	"view_matrix",
	"model_inv_trans_matrix",
	"projection_matrix",
	"diffuse_color",
	"ambient_color",
	"light_color",
	"specular_color",
	"light_direction",
	"camera_world_pos",
	"albedo_tex"
};

static char* const* get_uniform_names(const shader_type type)
{
	switch (type)
	{
		case shader_type::vertex_col:
		{
			return vertex_col_uniform_names;
		case shader_type::specular_diffuse:
			return specular_diffuse_uniform_names;
		default:
			return nullptr;
		}
	}
}

inline void get_uniforms_for_shader(const GLuint id, GLint * uniforms, shader_type type)
{
	const char* const* uniform_names = get_uniform_names(type);
	const int uniform_name_count = get_uniform_count(type);
	for (int i = 0; i < uniform_name_count; ++i)
	{
		uniforms[i] = glGetUniformLocation(id, uniform_names[i]);
	}
}


constexpr GLuint get_attribute_count(const shader_type type)
{
	switch (type)
	{
	case shader_type::vertex_col:
		return 2;
	case shader_type::specular_diffuse:
		return 3;
	default:
		return -1;
	}
}

static constexpr char* const vertex_col_attribute_names[] =
{
	"pos",
	"col"
};

static constexpr char* const specular_diffuse_attribute_names[] =
{
	"pos",
	"uv",
	"norm"
};

static char* const* get_attribute_names(const shader_type type)
{
	switch (type)
	{
		case shader_type::vertex_col:
		{
			return vertex_col_attribute_names;
		case shader_type::specular_diffuse:
			return specular_diffuse_attribute_names;
		default:
			return nullptr;
		}
	}
}

inline void get_attributes_for_shader(const GLuint id, GLint * attributes, shader_type type)
{
	const char* const* attribute_names = get_attribute_names(type);
	const int attribute_name_count = get_attribute_count(type);
	for (int i = 0; i < attribute_name_count; ++i)
	{
		attributes[i] = glGetAttribLocation(id, attribute_names[i]);
	}
}
