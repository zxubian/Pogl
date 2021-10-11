#pragma once

#include <glm/vec2.hpp>

struct Input_state
{
	/// <summary>
	/// Bit mask for which keyboard keys are pressed
	///
	/// ---- dsaw
	/// 0000 0000
	///
	/// </summary>
	unsigned char key_map;

	/// <summary>
	/// Current mouse pos
	/// </summary>
	glm::vec2 cursor_pos;

	/// <summary>
	/// Delta mouse pos
	/// </summary>
	glm::vec2 cursor_delta;

	static constexpr unsigned char w = 1 << 0;
	static constexpr unsigned char a = 1 << 1;
	static constexpr unsigned char s = 1 << 2;
	static constexpr unsigned char d = 1 << 3;

	bool get_w_pressed() const
	{
		return key_map & w;
	}

	bool get_a_pressed() const
	{
		return (key_map & a) >>1;
	}

	bool get_s_pressed() const
	{
		return (key_map & s) >> 2;
	}
	bool get_d_pressed() const
	{
		return (key_map & d) >> 3;
	}

	void set_w_pressed()
	{
		key_map ^= w;
	}

	void set_a_pressed()
	{
		key_map ^= a;
	}

	void set_s_pressed()
	{
		key_map ^= s;
	}

	void set_d_pressed()
	{
		key_map ^= d;
	}

	void set_w_released()
	{
		key_map &= ~w;
	}

	void set_a_released()
	{
		key_map &= ~a;
	}

	void set_s_released()
	{
		key_map &= ~s;
	}

	void set_d_released()
	{
		key_map &= ~d;
	}
};
