#pragma once

#include <gl/glew.h>

class Texture;

class Mesh
{
public:
	Mesh();
	void create_mesh
	(
		const GLfloat* vertices,
		const GLfloat* tex_coords,
		const GLfloat* normals,
		const unsigned char* colors,
		const unsigned int* indices,
		GLsizei vertex_count, GLsizei index_count,
		Texture** diffuse_maps,
		unsigned int diffuse_count
	);
	void Mesh::create_mesh
	(
		const GLfloat* vertices,
		const GLfloat* tex_coords,
		const GLfloat* normals,
		const unsigned int* indices,
		GLsizei vertex_count, GLsizei index_count,
		Texture** diffuse_maps,
		unsigned int diffuse_count
	);
	void clear_mesh();
	void positions_attribute_pointer(GLuint attribute_location)const;
	void normals_attribute_pointer(GLuint attribute_location)const;
	void colors_attribute_pointer(GLuint attribute_location)const;
	void texcoords_attribute_pointer(GLuint attribute_location)const;
	GLuint vao, vbo, ibo;
	GLsizei index_count;
	Texture** diffuse_maps;
	unsigned int diffuse_count;
	~Mesh();
private:
	GLintptr vertex_size;
	GLintptr texcoord_size;
	GLintptr normal_size;
	GLintptr color_size;
};
