#include "../include/PoglWindow.h"

Pogl_window::Pogl_window(int window_width, int window_height, Input_state& input):input_state(input)
{
	handle = nullptr;
	handle = glfwCreateWindow(window_width, window_height, "Pogl", nullptr, nullptr);
	if(handle == nullptr)
	{
		return;
	}
	glfwSetWindowUserPointer(handle, this);
	glfwSetKeyCallback(handle, handle_keyboard_input);
	glfwSetCursorPosCallback(handle, handle_mouse_input_first_time);
	glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Pogl_window::~Pogl_window()
{
	glfwDestroyWindow(handle);
	handle = nullptr;
}

void Pogl_window::handle_keyboard_input(GLFWwindow* handle, int key, int code, int action, int mode)
{
	Pogl_window* window = static_cast<Pogl_window*>(glfwGetWindowUserPointer(handle));

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(handle, GL_TRUE);
	}
	if(key >= 0 && key < 1024 && (action == GLFW_PRESS || action == GLFW_RELEASE))
	{
		const bool new_state = action == GLFW_PRESS;
		switch (key)
		{
			case GLFW_KEY_W:
				if(new_state)
				{
					window->input_state.set_w_pressed();
				}
				else
				{
					window->input_state.set_w_released();
				}
				break;
			case GLFW_KEY_A:
				if(new_state)
				{
					window->input_state.set_a_pressed();
				}
				else
				{
					window->input_state.set_a_released();
				}
				break;
			case GLFW_KEY_S:
				if(new_state)
				{
					window->input_state.set_s_pressed();
				}
				else
				{
					window->input_state.set_s_released();
				}
				break;
			case GLFW_KEY_D:
				if(new_state)
				{
					window->input_state.set_d_pressed();
				}
				else
				{
					window->input_state.set_d_released();
				}
				break;
			case GLFW_KEY_LEFT_SHIFT:
				glfwSetInputMode(handle, GLFW_CURSOR, new_state ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
				break;
			default: ;
		}
	}
}

void Pogl_window::handle_mouse_input_first_time(GLFWwindow* handle, double x_pos, double y_pos)
{
	Pogl_window* window = static_cast<Pogl_window*>(glfwGetWindowUserPointer(handle));

	Input_state& input_state = window->input_state;
	input_state.cursor_pos = glm::vec2(static_cast<GLfloat>(x_pos), static_cast<GLfloat>(y_pos));
	glfwSetCursorPosCallback(handle, handle_mouse_input);
}

void Pogl_window::handle_mouse_input(GLFWwindow* handle, double x_pos, double y_pos)
{
	Pogl_window* window = static_cast<Pogl_window*>(glfwGetWindowUserPointer(handle));

	Input_state& input_state = window->input_state;
	const GLfloat x = static_cast<GLfloat>(x_pos);
	const GLfloat y = static_cast<GLfloat>(y_pos);
	const glm::vec2 new_pos = glm::vec2(x,y);
	input_state.cursor_delta = new_pos - input_state.cursor_pos;
	input_state.cursor_pos = new_pos;
}
