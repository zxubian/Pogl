#pragma once

#include <gl/glew.h>

class mesh
{
public:
	mesh();

	void create_mesh
	(
		const GLfloat* vertices, const GLfloat* colors, const unsigned int* indices,
		GLsizei vertex_count, GLsizei index_count
	);

	void render_mesh() const;
	void clear_mesh();

	~mesh();
private:
	GLuint vao, ibo;
	GLuint* vbos;
	GLsizei index_count;
	bool has_colors;
};
