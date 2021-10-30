#include "../include/Mesh.h"

Mesh::Mesh()
{
	vao = 0;
	ibo = 0;
	index_count = 0;
	vbo = 0;
	vertex_size = 0;
	texcoord_size = 0;
	normal_size = 0;
	color_size = 0;
}

void Mesh::create_mesh
(
	const GLfloat* vertices,
	const GLfloat* tex_coords,
	const GLfloat* normals,
	const unsigned char* colors,
	const unsigned int* indices,
	GLsizei vertex_count, GLsizei index_count,
	Texture** diffuse_maps,
	unsigned int diffuse_count
)
{
	this->index_count = index_count;
	this->diffuse_maps = diffuse_maps;
	this->diffuse_count = diffuse_count;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		{
			vertex_size = vertex_count * 3 * sizeof(vertices[0]);
			texcoord_size = vertex_count * 2 * sizeof(tex_coords[0]);
			normal_size = vertex_count * 3 * sizeof(normals[0]);
			color_size = vertex_count * 4 * sizeof(colors[0]);

			glBufferData(GL_ARRAY_BUFFER, vertex_count*(vertex_size + texcoord_size + normal_size + color_size), nullptr, GL_STATIC_DRAW);

			// Merge attributes into single vbo
			// https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
			// https://www.haroldserrano.com/blog/loading-vertex-normal-and-uv-data-onto-opengl-buffers

			GLintptr offset = 0;
			glBufferSubData(GL_ARRAY_BUFFER, offset, vertex_size, vertices);
			offset += vertex_size;
			glBufferSubData(GL_ARRAY_BUFFER, offset, texcoord_size, tex_coords);
			offset += texcoord_size;
			glBufferSubData(GL_ARRAY_BUFFER, offset, normal_size, normals);
			offset += normal_size;
			glBufferSubData(GL_ARRAY_BUFFER, offset, color_size, colors);
			offset += color_size;
		}
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof indices[0], indices, GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
}


void Mesh::create_mesh
(
	const GLfloat* vertices,
	const GLfloat* tex_coords,
	const GLfloat* normals,
	const unsigned int* indices,
	GLsizei vertex_count, GLsizei index_count,
	Texture** diffuse_maps,
	unsigned int diffuse_count
)
{
	this->index_count = index_count;
	this->diffuse_maps = diffuse_maps;
	this->diffuse_count = diffuse_count;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		{
			vertex_size = vertex_count * 3 * sizeof(vertices[0]);
			texcoord_size = vertex_count * 2 * sizeof(tex_coords[0]);
			normal_size = vertex_count * 3 * sizeof(normals[0]);

			glBufferData(GL_ARRAY_BUFFER, vertex_count*(vertex_size + texcoord_size + normal_size + color_size), nullptr, GL_STATIC_DRAW);

			// Merge attributes into single vbo
			// https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
			// https://www.haroldserrano.com/blog/loading-vertex-normal-and-uv-data-onto-opengl-buffers

			GLintptr offset = 0;
			glBufferSubData(GL_ARRAY_BUFFER, offset, vertex_size, vertices);
			offset += vertex_size;
			glBufferSubData(GL_ARRAY_BUFFER, offset, texcoord_size, tex_coords);
			offset += texcoord_size;
			glBufferSubData(GL_ARRAY_BUFFER, offset, normal_size, normals);
			offset += normal_size;
		}
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof indices[0], indices, GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
}

void Mesh::clear_mesh()
{
	if (ibo != 0)
	{
		glDeleteBuffers(1, &ibo);
		ibo = 0;
	}
	if(vbo != 0)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
	if (vao != 0)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	index_count = 0;
}

void Mesh::positions_attribute_pointer(const GLuint attribute_location) const
{
	glVertexAttribPointer(attribute_location, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(0));
}

void Mesh::texcoords_attribute_pointer(const GLuint attribute_location) const
{
	glVertexAttribPointer(attribute_location, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(vertex_size));
}

void Mesh::normals_attribute_pointer(const GLuint attribute_location) const
{
	glVertexAttribPointer(attribute_location, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(vertex_size + texcoord_size));
}

void Mesh::colors_attribute_pointer(GLuint attribute_location) const
{
	glVertexAttribPointer(attribute_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)(vertex_size + texcoord_size + normal_size));
}

Mesh::~Mesh()
{
	clear_mesh();
}
