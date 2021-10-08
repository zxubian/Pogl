#include "Pogl.h"

#include <string.h>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Window Dimensions
constexpr GLint window_width = 800, window_height = 600;

GLuint vao, ibo, shader;
GLuint vbo[2];
GLint uniform_model_matrix, uniform_projection_matrix;

constexpr float tau = 6.28318531f;
constexpr float to_radians = tau / 360.f;

constexpr float tri_angle_increment = 0.005f;

float current_angle = 0.f;
float current_scale = 1.f;

// Vertex Shader:
static const char* vertex_shader =                       "\n\
#version 330                                             \n\
layout (location = 0) in vec3 pos;                       \n\
layout (location = 1) in vec4 col;                       \n\
\n\
out vec4 vertexColor;\n\
uniform mat4 model_matrix; \n\
uniform mat4 projection_matrix; \n\
\n\
void main(){											 \n\
    gl_Position = projection_matrix * model_matrix * vec4(pos.xyz, 1.0f);      \n\
	vertexColor = col;\n\
}                                                        \n\
";


// Fragment Shader:
static const char* fragment_shader =                     "\n\
#version 330                                             \n\
in vec4 vertexColor;\n\
out vec4 color;					                         \n\
void main(){                                             \n\
    color = vertexColor;                \n\
}                                                        \n\
";

mesh* create_tetrahedron()
{
	const glm::uint indicies[] = {
		0,3,1,
		1,3,2,
		2,3,0,
		0,1,2
	};

	const auto cos_theta = cos(120.f * to_radians);
	const auto sin_theta = sin(120.f * to_radians);

	const GLfloat vertices[] =
	{
		1.0f,0.f,0.f,
		cos_theta, 0,sin_theta,
		cos_theta, 0,-sin_theta,
		0.f,sqrt(2.f),0.f
	};

	GLfloat colors[] = 
	{
		1.0f,0.f,0.f,1.f,
		0.0f,1.f,0.f,1.f,
		0.0f,0.f,1.f,1.f,
		1.0f,1.f,0.f,1.f
	};

	const auto tetrahedron = new mesh();
	tetrahedron->create_mesh(vertices, colors, indicies, 12, 12);
	return tetrahedron;
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
	uniform_projection_matrix = glGetUniformLocation(shader, "projection_matrix");
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

	glEnable(GL_DEPTH_TEST);

	//Setup viewport size
	glViewport(0, 0, buffer_width, buffer_height);

	mesh* tetrahedron = create_tetrahedron();
	compile_shaders();

	glm::mat4 model_matrix(1.0f);
	glm::mat4 projection_matrix = glm::perspective(45.0f, (GLfloat)window_width / (GLfloat)window_height, 0.1f, 100.f);

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

		model_matrix = glm::mat4(1.0f);

		current_angle += tri_angle_increment * 100;
		if(current_angle >= 360)
		{
			current_angle -= 360;
		}
		model_matrix = glm::translate(model_matrix,glm::vec3(0, -1, -5));
		model_matrix = glm::scale(model_matrix, glm::vec3(current_scale, current_scale, current_scale));
		model_matrix = glm::rotate(model_matrix, current_angle * to_radians, glm::vec3(0, 1, 0));

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);
		glUniformMatrix4fv(uniform_model_matrix, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(uniform_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		tetrahedron->render_mesh();

		glUseProgram(0);

		glfwSwapBuffers(main_window);
	}

	return 0;
}
