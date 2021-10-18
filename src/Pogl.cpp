﻿#include "../include/Pogl.h"


// Window Dimensions
constexpr GLint window_width = 800, window_height = 600;

constexpr float tau = 6.28318531f;
constexpr float to_radians = tau / 360.f;

constexpr float tri_angle_increment = 0.005f;

float current_angle = 0.f;
float current_scale = 1.f;

const char* vertex_col_vert_path = "../../../data/shaders/vertex_col.vert";
const char* vertex_col_frag_path = "../../../data/shaders/vertex_col.frag";
const char* texture_path = "../../../data/textures/uv_checker.png";

// Fragment Shader:

Mesh* create_tetrahedron()
{
	const auto cos_theta = cos(120.f * to_radians);
	const auto sin_theta = sin(120.f * to_radians);

	const GLfloat vertices[] =
	{
		1.0f,0.f,0.f,
		cos_theta, 0,sin_theta,
		cos_theta, 0,-sin_theta,
		0.f,sqrt(2.f),0.f
	};

	constexpr glm::uint indicies[] = {
		0,3,1,
		2,1,3,
		2,3,0,
		0,1,2
	};

	constexpr GLfloat texcoord_0[] =
	{
		0.f,0.f,
		1.f,0.f,
		0.5f,0.f,
		0.5f,1.f
	};

	constexpr unsigned char colors[] =
	{
		255,0,0,0,
		0,255,0,255,
		0,0,255,255,
		255,255,0,255
	};

	const auto tetrahedron = new Mesh();
	tetrahedron->create_mesh(vertices, texcoord_0, colors, indicies, 4, 12);
	return tetrahedron;
}

void update_camera(Camera& camera, const Input_state& input_state, const double& delta_time)
{
	constexpr GLfloat camera_move_speed = 1.5f;
	constexpr GLfloat camera_rotate_speed = 50.f;

	glm::vec2 cursor_delta = input_state.cursor_delta;
	cursor_delta.y *= -1;
	cursor_delta *= delta_time * glm::radians(camera_rotate_speed);

	// https://gist.github.com/FreyaHolmer/650ecd551562352120445513efa1d952

	camera.yaw += cursor_delta.x;
	camera.pitch += cursor_delta.y;

	glm::vec4 translation = glm::vec4{ 0 };

	if(input_state.get_w_pressed())
	{
		translation += glm::vec4(0, 0, 1,0);
	}
	else if(input_state.get_s_pressed())
	{
		translation -= glm::vec4(0, 0, 1,0);
	}
	if(input_state.get_d_pressed())
	{
		translation += glm::vec4(1, 0, 0,0);
	}
	else if(input_state.get_a_pressed())
	{
		translation -= glm::vec4(1, 0, 0,0);
	}
	translation *= camera_move_speed * static_cast<GLfloat>(delta_time);
	translation = camera.transform_matrix* translation;
	camera.position += static_cast<glm::vec3>(translation);
	camera.recalculate_matricies();
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

	Input_state input_state = {};

	Pogl_window* window = new Pogl_window(window_width, window_height, input_state);

	if(!window->is_initialized())
	{
		delete window;
		glfwGetError(&errorMessage);
		glfwTerminate();
		std::cerr << errorMessage;
		return 1;
	}

	// Set context for GLEW to use
	// can have multiple windows and use this to switch between them
	glfwMakeContextCurrent(window->handle);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if(glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW init error";
		delete window;
		glfwTerminate();
		return 1;
	}

	// Get Buffer size information
	int buffer_width, buffer_height;
	glfwGetFramebufferSize(window->handle, &buffer_width, &buffer_height);

	glEnable(GL_DEPTH_TEST);

	//Setup viewport size
	glViewport(0, 0, buffer_width, buffer_height);

	Shader vertex_col_shader = Shader();
	vertex_col_shader.create_from_files(vertex_col_vert_path, vertex_col_frag_path);

	Camera camera =
	{
		glm::vec3(0,0,0),
		glm::vec3(0,0,1),
		glm::vec3(0,1,0)
	};

	double last_time = glfwGetTime();
	double delta_time = 0;
	double time;

	glfwSwapInterval(0);

	GLint vertex_col_uniforms[get_uniform_count(vertex_col)];

	get_uniforms_vertex_col(vertex_col_shader.id, vertex_col_uniforms);

	Program_render_data program_render_data
	{
		vertex_col_shader.id,
		vertex_col_uniforms
	};

	Per_frame_render_data per_frame_render_data
	{
		glm::perspective(45.0f, (GLfloat)window_width / (GLfloat)window_height, 0.1f, 100.f),
		camera.view_matrix
	};

	Transform vertex_col_transforms[2] =
	{
		Transform { glm::mat4x4{1}, },
		Transform { glm::translate(glm::mat4x4{1}, glm::vec3(0,0,1)), }
	};

	Mesh_render_data vertex_col_mesh
	{
		create_tetrahedron(),
		vertex_col_transforms,
		2
	};

	Vertex_col_data vertex_col
	{
		&vertex_col_mesh,
		1
	};

	Things_to_render things_to_render
	{
		vertex_col
	};

	// Loop until window closed
	while(!window->should_close())
	{
		time = glfwGetTime();
		delta_time = time - last_time;
		last_time = time;
		// Get & handle user events
		input_state.cursor_delta = glm::vec2{ 0 };

		glfwPollEvents(); // invokes callbacks in PoglWindow

		update_camera(camera, input_state, delta_time);

		per_frame_render_data.view_matrix = camera.view_matrix;

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render(&program_render_data, &per_frame_render_data, &things_to_render);

		glfwSwapBuffers(window->handle);
	}
	return 0;
}
