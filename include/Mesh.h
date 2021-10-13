#pragma once

#include <gl/glew.h>
#include <cstdlib>
#include <iostream>

class Mesh
{
public:
	Mesh();

	void create_mesh
	(
		const GLfloat* vertices, 
		const GLfloat* tex_coords,
		const unsigned char* colors, 
		const unsigned int* indices,
		GLsizei vertex_count, GLsizei index_count
	);

	void render_mesh() const;
	void clear_mesh();

	~Mesh();
private:
	GLuint vao, vbo, ibo;
	GLsizei index_count;
	bool has_colors, has_texcoords;
};
