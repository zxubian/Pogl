#include "../include/Shader.h"

Shader::Shader()
{
	_id = 0;
	model = 0;
	projection = 0;
	view = 0;
}

void Shader::create_from_string(const char* vertex_code, const char* fragment_code)
{
	_id = glCreateProgram();
	if(!id)
	{
		std::cerr << "Error creating Shader";
		return;
	}
	compile_and_add_shader(id, vertex_code, GL_VERTEX_SHADER);
	compile_and_add_shader(id, fragment_code, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar error_log[1024] = {0};

	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(id, sizeof(error_log), nullptr, error_log);
		fprintf(stderr, "Error linking program : '%s'\n", error_log);
	}

	glValidateProgram(id);
	glGetProgramiv(id, GL_VALIDATE_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(id, sizeof(error_log), nullptr, error_log);
		fprintf(stderr, "Error validating program : '%s'\n", error_log);
	}
	model = glGetUniformLocation(id, "model_matrix");
	projection = glGetUniformLocation(id, "projection_matrix");
	view = glGetUniformLocation(id, "view_matrix");
	main_tex = glGetUniformLocation(id, "main_tex");
}

void Shader::create_from_files(const char* vertex_path, const char* fragment_path)
{
	std::string vertex_string, fragment_string;

	if(!read_file(vertex_path, &vertex_string))
	{
		return;
	}
	if(!read_file(fragment_path, &fragment_string))
	{
		return;
	}
	create_from_string(&vertex_string[0], &fragment_string[0]);
}

void Shader::compile_and_add_shader(GLuint shader_id, const char* shader_code, GLenum shader_type)
{
	GLuint shader = glCreateShader(shader_type);
	const GLchar* code_chunks[1] = {shader_code};
	GLint codeLength[1];
	codeLength[0] = static_cast<GLint>(strlen(shader_code));
	glShaderSource(shader, 1, code_chunks, codeLength);
	glCompileShader(shader);

	GLint result = 0;
	GLchar error_log[1024] = {0};

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(shader, sizeof(error_log), nullptr, error_log);
		fprintf(stderr, "Error compiling %d Shader: '%s'\n", shader_type, error_log);
	}
	glAttachShader(shader_id, shader);
}

int Shader::read_file(const char* file_path, std::string* content) const
{
	std::ifstream file_stream;
	file_stream.open(file_path);
	if(!file_stream)
	{
		fprintf(stderr, "Error opening file: '%s'\n", file_path);
		std::cerr << "Error: " << strerror(errno) << "\n";
		return 0;
	}
	std::string line;
	while(!file_stream.eof())
	{
		std::getline(file_stream, line);
		content->append(line);
		content->append("\n");
	}
	file_stream.close();
	return 1;
}

GLuint Shader::get_projection_location() const
{
	return uniform_projection;
}

GLuint Shader::get_model_location() const
{
	return uniform_model;
}

void Shader::clear_shader()
{
	if(id != 0)
	{
		glDeleteProgram(id);
		_id = 0;
	}
	model = 0;
	projection = 0;
	view = 0;
}

Shader::~Shader()
{
	clear_shader();
}
