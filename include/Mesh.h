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
		const GLfloat* vertices, const unsigned char* colors, const unsigned int* indices,
		const unsigned short* tex_coords,
		const GLsizei vertex_count, const GLsizei index_count
	);

	void render_mesh() const;
	void clear_mesh();

	~Mesh();
private:
	GLuint vao, ibo;
	GLuint* vbos;
	GLsizei index_count;
	bool has_colors, has_texcoords;
};
