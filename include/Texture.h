#pragma once

#include <gl\glew.h>
#include <iostream>

#include "../include/stb_image.h"

class Texture
{
public:
	Texture();
	Texture(const char* file_path);
	bool load_texture();
	void clear_texture();
	const GLuint& id = texture_id;
	~Texture();

private:
	GLuint texture_id;
	int width, height, bit_depth;
	const char* file_path;
};
