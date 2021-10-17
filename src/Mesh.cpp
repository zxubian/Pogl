#include "../include/Mesh.h"

Mesh::Mesh()
{
	vao = 0;
	ibo = 0;
	index_count = 0;
	vbo = 0;
	has_colors = false;
	has_texcoords = false;
}

void Mesh::create_mesh
(
	const GLfloat* vertices, 
	const GLfloat* tex_coords,
	const unsigned char* colors, 
	const unsigned int* indices,
	const GLsizei vertex_count, const GLsizei index_count
)
{
	this->index_count = index_count;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	{
		has_colors = colors != nullptr;
		has_texcoords = tex_coords != nullptr;

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		{
			GLintptr vertex_size = vertex_count * 3 * sizeof(vertices[0]);
			GLintptr texcoord_size = vertex_count * 2 * sizeof(tex_coords[0]);
			GLintptr color_size = vertex_count * 4 * sizeof(colors[0]);
			glBufferData(GL_ARRAY_BUFFER, vertex_count*(vertex_size + texcoord_size + color_size), nullptr, GL_STATIC_DRAW);

			// Merge attributes into single vbo
			// https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices 
			// https://www.haroldserrano.com/blog/loading-vertex-normal-and-uv-data-onto-opengl-buffers	

			glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_size, vertices);
			glBufferSubData(GL_ARRAY_BUFFER, vertex_size, texcoord_size, tex_coords);
			glBufferSubData(GL_ARRAY_BUFFER, vertex_size + texcoord_size, color_size, colors);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(0));
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(vertex_size));
			glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)(vertex_size + texcoord_size));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
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

Mesh::~Mesh()
{
	clear_mesh();
}
