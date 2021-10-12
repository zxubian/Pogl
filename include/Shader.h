#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>

class Shader
{
public:
	Shader();

	void create_from_string (const char* vertex_code, const char* fragment_code);
	void create_from_files (const char* vertex_path, const char* fragment_path);

	GLuint get_projection_location() const;
	GLuint get_model_location() const;

	void clear_shader();

	const GLuint& id = _id;
	const GLint& uniform_projection = projection;
	const GLint& uniform_model = model;
	const GLint& uniform_view = view;
	const GLint& uniform_main_tex = main_tex;

	~Shader();

private:
	GLuint _id;
	GLint projection, model, view, main_tex;
	static void compile_and_add_shader(GLuint shader_id, const char* shader_code, GLenum shader_type);
	int read_file(const char* file_path, std::string* content) const;
};