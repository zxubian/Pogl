#include <gl/glew.h>
#include <glm/glm.hpp>

#include "../include/Shapes.h"
#include "../include/Utils.h"

Mesh* create_tetrahedron()
{
	const GLfloat cos_theta = cos(120.f * to_radians);
	const GLfloat sin_theta = sin(120.f * to_radians);

	GLint vertex_count = 4;
	GLint index_count = 12;

	GLfloat vertices[] =
	{
		1.0f,0.f,0.f,
		cos_theta, 0,sin_theta,
		cos_theta, 0,-sin_theta,
		0.f,sqrt(2.f),0.f
	};

	GLfloat texcoord_0[] =
	{
		0.f,0.f,
		1.f,0.f,
		0.5f,0.f,
		0.5f,1.f
	};

	unsigned char colors[] =
	{
		255,0,0,255,
		0,255,0,255,
		0,0,255,255,
		255,255,0,255
	};

	glm::uint indices[] = {
		0,3,1,
		2,1,3,
		2,3,0,
		0,1,2
	};

	glm::vec3* normals = new glm::vec3[vertex_count];

	for(GLuint i = 0; i < vertex_count; ++i)
	{
		normals[i] = glm::vec3{ 0 };
	}

	generate_normals(reinterpret_cast<glm::vec3*>(vertices), indices, vertex_count, index_count, normals);
	const auto tetrahedron = new Mesh();
	tetrahedron->create_mesh(vertices, texcoord_0, reinterpret_cast<GLfloat*>(normals), colors, indices, vertex_count, index_count);
	delete[](normals);
	return tetrahedron;
}


