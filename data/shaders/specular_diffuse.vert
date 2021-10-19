#version 330
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 3) in vec3 norm;

out vec2 texcoord_0;
out vec3 normal;
out vec4 world_pos;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 mv_inv_trans_matrix;
uniform mat4 projection_matrix;

void main()
{
	texcoord_0 = uv;
	normal = (mv_inv_trans_matrix * vec4(norm.xyz, 0)).xyz;
	world_pos = model_matrix * vec4(pos.xyz, 1.0f);
	world_pos.w = 1;
    gl_Position = projection_matrix * view_matrix * world_pos;
}
