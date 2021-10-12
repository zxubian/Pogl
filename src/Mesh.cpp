#include "../include/Mesh.h"

Mesh::Mesh()
{
	vao = 0;
	ibo = 0;
	index_count = 0;
	vbos = nullptr;
	has_colors = false;
	has_texcoords = false;
}

void Mesh::create_mesh
(
	const GLfloat* vertices, const unsigned char* colors, const unsigned int* indices,
	const unsigned short* tex_coords,
	const GLsizei vertex_count, const GLsizei index_count
)
{
	//TODO: @performance merge attributes into single vbo
	// https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices 

	this->index_count = index_count;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	{
		has_colors = colors != nullptr;
		has_texcoords = tex_coords != nullptr;

		GLsizei vbo_count = 1;
		if (has_colors)
		{
			vbo_count++;
		}
		if (has_texcoords)
		{
			vbo_count++;
		}
		vbos = static_cast<GLuint*>(malloc(sizeof(GLuint) * vbo_count));
		if (vbos == nullptr)
		{
			std::cerr << "Failed to allocate space for vbos.";
			return;
		}
		glGenBuffers(vbo_count, vbos);
		GLsizei vbo_index = 0;
		glBindBuffer(GL_ARRAY_BUFFER, vbos[vbo_index]);
		{
			glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof vertices[0], vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(0);
		}
		vbo_index++;
		if (has_texcoords)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbos[vbo_index]);
			{
				glBufferData(GL_ARRAY_BUFFER, vertex_count * 2 * sizeof tex_coords[0], tex_coords, GL_STATIC_DRAW);
				glVertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_FALSE, 0, nullptr);
				glEnableVertexAttribArray(1);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			vbo_index++;
		}

		if (has_colors)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo_index);
			{
				glBufferData(GL_ARRAY_BUFFER, vertex_count * 4 * sizeof(colors[1]), colors, GL_STATIC_DRAW);
				glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);
				glEnableVertexAttribArray(2);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			vbo_index++;
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof indices[0], indices, GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
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
	if (ibo != 0)
	{
		glDeleteBuffers(1, &ibo);
		ibo = 0;
	}
	if (vbos != nullptr)
	{
		auto buffer_count = has_colors ? 2 : 1;
		glDeleteBuffers(buffer_count, vbos);
		free(vbos);
		vbos = nullptr;
	}
	if (vao != 0)
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
