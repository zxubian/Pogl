#version 330
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 col;

out vec4 vertex_color;

uniform mat4 vp_matrix;
uniform mat4 model_matrix;

void main()
{
    gl_Position = vp_matrix * model_matrix * vec4(pos.xyz, 1.0f);
	vertex_color = col;
}
