
#version 330                                             

in vec4 vertex_color;
in vec2 texcoord_0;

out vec4 color;	

uniform sampler2D main_tex;

void main()
{
	 color = texture(main_tex, texcoord_0) * vertex_color;                
}                                      
