#version 330                                             
layout (location = 0) in vec3 pos;                       
layout (location = 1) in vec4 col;                       
out vec4 vertexColor;
uniform mat4 model_matrix;
uniform mat4 projection_matrix;
void main()
{											 
    gl_Position = projection_matrix * model_matrix * vec4(pos.xyz, 1.0f);
	vertexColor = col;
}
