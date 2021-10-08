#include "Mesh.h"

#include <cstdlib>

Mesh::Mesh()
{
	vao = 0;
	ibo = 0;
	index_count = 0;
	vbos = nullptr;
	has_colors = false;
}

void Mesh::create_mesh
(
	const GLfloat* vertices, const GLfloat* colors, const unsigned int* indices, 
	const GLsizei vertex_count, const GLsizei index_count
)
{
	this->index_count = index_count;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	has_colors = colors != nullptr;

	const GLsizei vbo_count = has_colors ? 2 : 1;
	vbos = static_cast<GLuint*>(malloc(sizeof(GLuint) * vbo_count));
	if(vbos == nullptr)
	{
		return;
	}
	glGenBuffers(vbo_count, vbos);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	{
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof vertices[0], vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
	}

	if (has_colors)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
		{
			glBufferData(GL_ARRAY_BUFFER, vertex_count * 4 * sizeof(colors[1]), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(1);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof indices[0], indices, GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	auto error = glGetError();
}

void Mesh::render_mesh() const
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::clear_mesh()
{
	if(ibo != 0)
	{
		glDeleteBuffers(1, &ibo);
		ibo = 0;
	}
	if(vbos != nullptr)
	{
		auto buffer_count = has_colors ? 2 : 1;
		glDeleteBuffers(buffer_count, vbos);
		free(vbos);
		vbos = nullptr;
	}
	if(vao != 0)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	index_count = 0;
}

Mesh::~Mesh()
{
	clear_mesh();
}
