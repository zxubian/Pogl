#version 330
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 3) in vec3 norm;

out vec2 texcoord_0;
out vec3 normal;

uniform mat4 mv_matrix;
uniform mat4 projection_matrix;

void main()
{
    gl_Position = projection_matrix * mv_matrix * vec4(pos.xyz, 1.0f);
	texcoord_0 = uv;
	normal = norm;
}
