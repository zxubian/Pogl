#include "../include/Pogl.h"

// Window Dimensions
constexpr GLint window_width = 1240, window_height = 1020;

constexpr float tri_angle_increment = 0.005f;

float current_angle = 0.f;
float current_scale = 1.f;

const char* vertex_col_vert_path = "./data/shaders/vertex_col.vert";
const char* vertex_col_frag_path = "./data/shaders/vertex_col.frag";
const char* specular_diffuse_vert_path = "./data/shaders/specular_diffuse.vert";
const char* specular_diffuse_frag_path = "./data/shaders/specular_diffuse.frag";
const char* texture_path = "./data/textures/uv_checker.png";

// Fragment Shader:

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

	if (input_state.get_w_pressed())
	{
		translation += glm::vec4(0, 0, 1, 0);
	}
	else if (input_state.get_s_pressed())
	{
		translation -= glm::vec4(0, 0, 1, 0);
	}
	if (input_state.get_d_pressed())
	{
		translation += glm::vec4(1, 0, 0, 0);
	}
	else if (input_state.get_a_pressed())
	{
		translation -= glm::vec4(1, 0, 0, 0);
	}
	translation *= camera_move_speed * static_cast<GLfloat>(delta_time);
	translation = camera.transform_matrix * translation;
	camera.position += static_cast<glm::vec3>(translation);
	camera.recalculate_matricies();
}

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

int main()
{
	// Init GLFW
	auto status = glfwInit();
	const char* errorMessage;
	if (!status)
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


	// During init, enable debug output
	Input_state input_state = {};

	Pogl_window* window = new Pogl_window(window_width, window_height, input_state);

	if (!window->is_initialized())
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

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW init error";
		delete window;
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	// Get Buffer size information
	int buffer_width, buffer_height;
	glfwGetFramebufferSize(window->handle, &buffer_width, &buffer_height);

	glEnable(GL_DEPTH_TEST);

	//Setup viewport size
	glViewport(0, 0, buffer_width, buffer_height);

	Shader vertex_col_shader = Shader();
	vertex_col_shader.create_from_files(vertex_col_vert_path, vertex_col_frag_path);

	Shader specular_diffuse_shader = Shader();
	specular_diffuse_shader.create_from_files(specular_diffuse_vert_path, specular_diffuse_frag_path);

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

	Program_render_data program_render_data
	{
		vertex_col_shader.id,
		specular_diffuse_shader.id,
	};

	Transform vertex_col_transforms[1] =
	{
		Transform { glm::translate(glm::mat4x4{1}, glm::vec3(-3,-2,5))},
	};

	Transform tetrahedron_transforms[1] =
	{
		Transform { glm::translate(glm::mat4x4{1}, glm::vec3(3,-2,5))}
	};

	glm::mat4x4 plane_transform = glm::translate(glm::mat4x4{ 1 }, glm::vec3(0, -2, 0));
	plane_transform = glm::rotate(plane_transform, glm::radians(-90.f), glm::vec3(1, 0, 0));
	plane_transform = glm::scale(plane_transform, glm::vec3(100));

	Transform plane_transforms[1] =
	{
		Transform {plane_transform}
	};

	Mesh* tetrahedron = create_tetrahedron();
	Mesh* plane = create_plane();
	Texture* texture = new Texture(texture_path);
	texture->load_texture();

	Mesh_render_data vertex_col_mesh
	{
		tetrahedron,
		vertex_col_transforms,
		1
	};

	Mesh_render_data specular_diffuse_meshes[] =
	{
		{
			tetrahedron,
			tetrahedron_transforms,
			1
		},
		{
			plane,
			plane_transforms,
			1
		}
	};

	Directional_light directional_light
	{
		glm::vec3(-0.5, -1, 1),
		glm::vec4(1,1,1,0.3),
	};

	GLuint point_light_count = 1;

	Point_light* point_lights = new Point_light[point_light_count]
	{
		{
			{0,0,0},
			{1,1,1},
			100,
			3
		}
	};

	Light_data light_data
	{
		directional_light,
		point_lights,
		point_light_count
	};

	Specular_diffuse_instance_render_data tetrahedron1
	{
		glm::vec4(1,1,1,1),
		glm::vec3(1,1,1),
		glm::vec3(0.1,0.1,0.1),
		glm::vec4(1,1,64,1),
		texture
	};

	Specular_diffuse_instance_render_data* tetrahedron_data = new Specular_diffuse_instance_render_data[1]
	{
		tetrahedron1
	};

	Specular_diffuse_instance_render_data plane1
	{
		glm::vec4(1,1,1,1),
		glm::vec3(1,1,1),
		glm::vec3(0.1,0.1,0.1),
		glm::vec4(1,1,64,1),
		texture
	};

	Specular_diffuse_instance_render_data* plane_data = new Specular_diffuse_instance_render_data[1]
	{
		plane1
	};

	Specular_diffuse_instance_render_data** specular_diffuse_instance_data = new Specular_diffuse_instance_render_data * [2]
	{
		tetrahedron_data,
		plane_data
	};

	Specular_diffuse_data specular_diffuse
	{
		specular_diffuse_meshes,
		specular_diffuse_instance_data,
		2,
	};

	Vertex_col_data vertex_col
	{
		&vertex_col_mesh,
		1
	};

	Things_to_render things_to_render
	{
		vertex_col,
		specular_diffuse
	};

	Per_frame_render_data per_frame_render_data
	{
		glm::perspective(45.0f, static_cast<GLfloat>(window_width) / static_cast<GLfloat>(window_height), 0.1f, 100.f),
		camera.view_matrix,
		light_data
	};


	// Loop until window closed
	while (!window->should_close())
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

	delete[] tetrahedron_data;
	delete[] specular_diffuse_instance_data;
	delete[] point_lights;

	return 0;
}
