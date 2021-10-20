#pragma once

#include <glm/glm.hpp>
#include <gl/glew.h>

void generate_normals(const glm::vec3* positions, const GLuint* indicies, GLuint vertex_count, GLuint index_count, glm::vec3* normals)
{
	GLuint triangle_count = index_count / 3;
	GLuint index = 0;
	GLfloat* per_vertex_averaging_factor = new GLfloat[vertex_count];
	for(GLuint a = 0; a < vertex_count; ++a)
	{
		per_vertex_averaging_factor[a] = 0;
	}
	for(GLuint t = 0; t < triangle_count; ++t, ++index)
	{
		const GLuint a_index = indicies[index];
		const GLuint b_index = indicies[++index];
		const GLuint c_index = indicies[++index];
		glm::vec3 a = positions[a_index];
		glm::vec3 b = positions[b_index];
		glm::vec3 c = positions[c_index];
		glm::vec3 ba = b - a;
		glm::vec3 ca = c - a;
		glm::vec3 normal = glm::cross(ca, ba);
		const GLfloat area = glm::length(normal)/2;
		normal *= area;
		normals[a_index] += normal;
		normals[b_index] += normal;
		normals[c_index] += normal;
		per_vertex_averaging_factor[a_index] += area;
		per_vertex_averaging_factor[b_index] += area;
		per_vertex_averaging_factor[c_index] += area;
	}
	for(GLuint v = 0; v < vertex_count; ++v)
	{
		normals[v] /= per_vertex_averaging_factor[v];
		normals[v] = glm::normalize(normals[v]);
	}
	delete[] per_vertex_averaging_factor;
}
