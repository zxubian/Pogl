#include "../include/Texture.h"

#include <iostream>

Texture::Texture()
{
	texture_id = 0;
	width = 0;
	height = 0;
	bit_depth = 0;
	file_path = "";
}

Texture::Texture(const char* file_path)
{
	texture_id = 0;
	width = 0;
	height = 0;
	bit_depth = 0;
	this->file_path = file_path;
}

bool Texture::load_texture()
{
	unsigned char* texture_data = stbi_load(file_path, &width, &height, &bit_depth, 0);
	if(texture_data == nullptr)
	{
		std::cerr << "Failed to open texture " << file_path << "\n";
		return false;
	}
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(texture_data);
	const GLenum success= glGetError();
	if(!success == GL_NO_ERROR)
	{
		std::cerr << "Error when importing texture " << file_path << " " << glewGetErrorString(success) << "\n";
		return false;
	}
	return true;
}

void Texture::clear_texture()
{
	glDeleteTextures(1, &texture_id);
}

Texture::~Texture()
{
	if(texture_id != 0)
	{
		clear_texture();
	}
	texture_id = 0;
	width = 0;
	height = 0;
	bit_depth = 0;
	file_path = "";
}
