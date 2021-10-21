#version 330
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 3) in vec3 norm;

out V2F{
	vec2 texcoord_0;
	vec3 normal;
	vec4 world_pos;
} v2f;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 model_inv_trans_matrix;
uniform mat4 projection_matrix;

void main()
{
	v2f.texcoord_0 = uv;
	v2f.normal = (model_inv_trans_matrix * vec4(norm.xyz, 0)).xyz;
	vec4 world_pos = model_matrix * vec4(pos.xyz, 1.0f);
	world_pos.w = 1;
	v2f.world_pos = world_pos;
    gl_Position = projection_matrix * view_matrix * world_pos;
}
