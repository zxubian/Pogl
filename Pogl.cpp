#include "Pogl.h"

#include <string.h>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Window Dimensions
constexpr GLint window_width = 800, window_height = 800;

GLuint vao, vbo, shader;
GLint uniform_model_matrix;
constexpr float tau = 6.28318531f;
constexpr float to_radians = tau / 360.f;


constexpr float tri_offset_max = 0.7f;
constexpr float tri_offset_increment = 0.005f;
float tri_offset = 0.f;
float direction = 1.f;

float current_angle = 0.f;
float current_scale = 1.f;



// Vertex Shader:
static const char* vertex_shader =                       "\n\
#version 330                                             \n\
layout (location = 0) in vec3 pos;                       \n\
uniform mat4 model_matrix; \n\
void main(){											 \n\
    gl_Position = model_matrix * vec4(pos.xyz, 1.0f);      \n\
}                                                        \n\
";


// Fragment Shader:
static const char* fragment_shader =                     "\n\
#version 330                                             \n\
out vec4 color;					                         \n\
void main(){                                             \n\
    color = vec4(1.0f, 0.7f, 0.8f, 1.0f);                \n\
}                                                        \n\
";

void create_triangle()
{
	GLfloat vertices[] = {
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// from now on, any code using vertex arrays will use the one who's id is vao;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void compile_and_add_shader(GLuint program, const char* shader_code, GLenum shader_type)
{
	GLuint shader = glCreateShader(shader_type);
	const GLchar* code_chunks[1] = {shader_code};
	GLint codeLength[1];
	codeLength[0] = static_cast<GLint>(strlen(shader_code));
	glShaderSource(shader, 1, code_chunks, codeLength);
	glCompileShader(shader);

	GLint result = 0;
	GLchar error_log[1024] = {0};
	auto v = tau;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(shader, sizeof(error_log), nullptr, error_log);
		fprintf(stderr, "Error compiling %d shader: '%s'\n", shader_type, error_log);
	}

	glAttachShader(program, shader);
}

void compile_shaders()
{
	shader = glCreateProgram();
	if(!shader)
	{
		std::cerr << "Error creating shader";
		return;
	}
	compile_and_add_shader(shader, vertex_shader, GL_VERTEX_SHADER);
	compile_and_add_shader(shader, fragment_shader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar error_log[1024] = {0};

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(shader, sizeof(error_log), nullptr, error_log);
		fprintf(stderr, "Error linking program : '%s'\n", error_log);
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(shader, sizeof(error_log), nullptr, error_log);
		fprintf(stderr, "Error validating program : '%s'\n", error_log);
	}

	uniform_model_matrix = glGetUniformLocation(shader, "model_matrix");
}

int main()
{
	// Init GLFW
	auto status = glfwInit();
	const char* errorMessage;
	if(!status)
	{
		glfwGetError(&errorMessage);
		glfwTerminate();
		std::cerr << errorMessage;
		return 1;
	}

	//Setup GLFW window properties
	//OpenGL Version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//Core profile == No backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow Forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* main_window = glfwCreateWindow(window_width, window_height, "Pogl", nullptr, nullptr);

	if(!main_window)
	{
		glfwGetError(&errorMessage);
		glfwTerminate();
		std::cerr << errorMessage;
		return 1;
	}

	// Set context for GLEW to use
	// can have multiple windows and use this to switch between them
	glfwMakeContextCurrent(main_window);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if(glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW init error";
		glfwDestroyWindow(main_window);
		glfwTerminate();
		return 1;
	}

	// Get Buffer size information
	int buffer_width, buffer_height;
	glfwGetFramebufferSize(main_window, &buffer_width, &buffer_height);

	//Setup viewport size
	glViewport(0, 0, buffer_width, buffer_height);

	create_triangle();
	compile_shaders();

	glm::mat4 model_matrix(1.0f);

	// Loop until window closed
	while(!glfwWindowShouldClose(main_window))
	{
		// Get & handle user events
		glfwPollEvents();

		if( glfwGetMouseButton(main_window, 0) == GLFW_PRESS)
		{
			current_scale += 0.1f;
		}
		else if( glfwGetMouseButton(main_window, 1) == GLFW_PRESS)
		{
			current_scale -= 0.1f;
		}

		tri_offset += tri_offset_increment * direction;
		if(abs(tri_offset) >= tri_offset_max)
		{
			direction *= -1;
		}

		model_matrix = glm::mat4(1.0f);

		current_angle += tri_offset_increment * 100;
		if(current_angle >= 360)
		{
			current_angle -= 360;
		}

		model_matrix = glm::scale(model_matrix, glm::vec3(current_scale, current_scale, current_scale));
		model_matrix = glm::translate(model_matrix, glm::vec3(tri_offset, 0.f, 0.f));
		model_matrix = glm::rotate(model_matrix, current_angle * to_radians, glm::vec3(0, 0, 1));

		//

		//

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glUniformMatrix4fv(uniform_model_matrix, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(main_window);
	}

	return 0;
}
