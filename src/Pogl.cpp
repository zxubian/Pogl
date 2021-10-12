#include "../include/Pogl.h"

// Window Dimensions
constexpr GLint window_width = 800, window_height = 600;

constexpr float tau = 6.28318531f;
constexpr float to_radians = tau / 360.f;

constexpr float tri_angle_increment = 0.005f;

float current_angle = 0.f;
float current_scale = 1.f;

const char* vertex_shader_path = "../../../data/shaders/shader.vert";
const char* fragment_shader_path = "../../../data/shaders/shader.frag";
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

	const glm::uint indicies[] = {
		0,3,1,
		2,1,3,
		2,3,0,
		0,1,2
	};

	unsigned short texcoord_0[] =
	{
		0,0,
		1,0,
		0.5,0,
		0.5,1
	};

	unsigned char colors[] =
	{
		255,0,0,0,
		0,255,0,255,
		0,0,255,255,
		255,255,0,255
	};

	const auto tetrahedron = new Mesh();
	tetrahedron->create_mesh(vertices, colors, indicies, texcoord_0, 12, 12);
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

	std::vector<Mesh*> meshes = std::vector<Mesh*>();
	meshes.push_back(create_tetrahedron());
	meshes.push_back(create_tetrahedron());
	Shader shader = Shader();
	shader.create_from_files(vertex_shader_path, fragment_shader_path);

	Texture* texture = new Texture(texture_path);

	texture->load_texture();

	glm::mat4 model_matrix(1.0f);
	glm::mat4 projection_matrix = glm::perspective(45.0f, (GLfloat)window_width / (GLfloat)window_height, 0.1f, 100.f);

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

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader.id);

		glm::vec3 offset = glm::vec3{2, 0, 0};
		glm::float32 i = 0;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->id);
		for (const auto mesh : meshes)
		{
			model_matrix = glm::mat4(1.0f);
			model_matrix = glm::translate(model_matrix,glm::vec3(0, 0, 5) + (offset * i));
			model_matrix = glm::scale(model_matrix, glm::vec3(current_scale, current_scale, current_scale));
			model_matrix = glm::rotate(model_matrix, current_angle * to_radians, glm::vec3(0, 1, 0));
			glUniformMatrix4fv(shader.uniform_model, 1, GL_FALSE, glm::value_ptr(model_matrix));
			glUniformMatrix4fv(shader.uniform_view, 1, GL_FALSE, glm::value_ptr(camera.view_matrix));
			glUniformMatrix4fv(shader.uniform_projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));
			mesh -> render_mesh();
			i += 1;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(0);

		glUseProgram(0);

		glfwSwapBuffers(window->handle);
	}

	return 0;
}
