#include "../include/Pogl.h"


// Window Dimensions
constexpr GLint window_width = 800, window_height = 600;

constexpr float tau = 6.28318531f;
constexpr float to_radians = tau / 360.f;

constexpr float tri_angle_increment = 0.005f;

float current_angle = 0.f;
float current_scale = 1.f;

const char* vertex_col_vert_path = "../../../data/shaders/vertex_col.vert";
const char* vertex_col_frag_path = "../../../data/shaders/vertex_col.frag";
const char* specular_diffuse_vert_path = "../../../data/shaders/specular_diffuse.vert";
const char* specular_diffuse_frag_path = "../../../data/shaders/specular_diffuse.frag";
const char* texture_path = "../../../data/textures/uv_checker.png";

// Fragment Shader:

Mesh* create_tetrahedron()
{
	const auto cos_theta = cos(120.f * to_radians);
	const auto sin_theta = sin(120.f * to_radians);

	GLfloat vertices[] =
	{
		1.0f,0.f,0.f,
		cos_theta, 0,sin_theta,
		cos_theta, 0,-sin_theta,
		0.f,sqrt(2.f),0.f
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
		255,0,0,255,
		0,255,0,255,
		0,0,255,255,
		255,255,0,255
	};

	GLuint index_count = 12;
	GLuint vertex_count = 4;

	constexpr glm::uint indices[] = {
		0,3,1,
		2,1,3,
		2,3,0,
		0,1,2
	};

	glm::vec3* normals = new glm::vec3[vertex_count];

	for(GLuint i = 0; i < vertex_count; ++i)
	{
		normals[i] = glm::vec3{ 0 };
	}

	generate_normals(reinterpret_cast<glm::vec3*>(vertices), indices, vertex_count, index_count, normals);
	const auto tetrahedron = new Mesh();
	tetrahedron->create_mesh(vertices, texcoord_0, reinterpret_cast<GLfloat*>(normals), colors, indices, 4, 12);
	delete[](normals);
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

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
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


	// During init, enable debug output

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

	glEnable( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, 0 );

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

	GLint vertex_col_uniforms[get_uniform_count(shader_type::vertex_col)];
	get_uniforms_for_shader(vertex_col_shader.id, vertex_col_uniforms, shader_type::vertex_col);
	GLint specular_diffuse_uniforms[get_uniform_count(shader_type::specular_diffuse)];
	get_uniforms_for_shader(specular_diffuse_shader.id, specular_diffuse_uniforms, shader_type::specular_diffuse);

	GLint vertex_col_attributes[get_attribute_count(shader_type::vertex_col)];
	get_attributes_for_shader(vertex_col_shader.id, vertex_col_attributes, shader_type::vertex_col);
	GLint specular_diffuse_attributes[get_attribute_count(shader_type::specular_diffuse)];
	get_attributes_for_shader(specular_diffuse_shader.id, specular_diffuse_attributes, shader_type::specular_diffuse);

	Program_render_data program_render_data
	{
		vertex_col_shader.id,
		vertex_col_uniforms,
		vertex_col_attributes,
		specular_diffuse_shader.id,
		specular_diffuse_uniforms,
		specular_diffuse_attributes
	};

	Transform vertex_col_transforms[1] =
	{
		Transform { glm::translate(glm::mat4x4{1}, glm::vec3(-3,-2,5))},
	};

	Transform specular_diffuse_transforms[1] =
	{
		Transform { glm::translate(glm::mat4x4{1}, glm::vec3(3,-2,5))}
	};

	Mesh* tetrahedron = create_tetrahedron();

	Texture* texture = new Texture(texture_path);
	texture->load_texture();

	Mesh_render_data vertex_col_mesh
	{
		tetrahedron,
		vertex_col_transforms,
		1
	};

	Mesh_render_data specular_diffuse_mesh
	{
		tetrahedron,
		specular_diffuse_transforms,
		1
	};

	Light_data light_data
	{
		glm::vec4(1,1,1,0.1),
		glm::vec3(-0.5, -1, 1),
		glm::vec4(1,1,1,1),
		glm::vec4(1,1,1,0.91)
	};

	Specular_diffuse_instance_render_data instance1_data
	{
		texture,
		glm::vec4(1,1,1,1),
		32
	};

	Specular_diffuse_instance_render_data* tetrahedron_data = new Specular_diffuse_instance_render_data[1]
	{
		instance1_data
	};

	Specular_diffuse_instance_render_data** specular_diffuse_instance_data = new Specular_diffuse_instance_render_data*[1]
	{
		tetrahedron_data
	};

	Specular_diffuse_data specular_diffuse
	{
		&specular_diffuse_mesh,
		specular_diffuse_instance_data,
		1,
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

	delete[] tetrahedron_data;
	delete[] specular_diffuse_instance_data;
	return 0;
}
