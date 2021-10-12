#version 330
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 col;

out vec4 vertex_color;
out vec2 texcoord_0;

uniform mat4 model_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(pos.xyz, 1.0f);
	vertex_color = col;
	texcoord_0 = uv;
}
