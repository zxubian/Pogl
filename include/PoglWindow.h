#pragma once

#include <GLFW/glfw3.h>

#include "InputState.h"

class Pogl_window
{
public:
	Pogl_window(int width, int height, Input_state& input_state);
	~Pogl_window();
	bool is_initialized() const { return handle != nullptr; }
	bool should_close() const { return glfwWindowShouldClose(handle); }
	GLFWwindow* handle;
private:
	static void handle_keyboard_input(GLFWwindow* handle, int key, int code, int action, int mode);
	static void handle_mouse_input_first_time(GLFWwindow* handle, double x_pos, double y_pos);
	static void handle_mouse_input(GLFWwindow* handle, double x_pos, double y_pos);
	Input_state& input_state;
};