
#version 330

in vec2 texcoord_0;
in vec3 normal;
in vec4 world_pos;

out vec4 color;

uniform vec4 diffuse_color;
uniform vec4 ambient_color;
uniform vec4 light_color;
uniform vec3 specular_color;
uniform vec3 light_direction;
uniform vec3 camera_world_pos;
uniform float specular_power;
uniform sampler2D albedo_tex;

vec3 diffuse(vec3 normal, vec3 light_direction)
{
	vec3 direct_color = light_color.xyz * light_color.w * clamp(dot(normal,light_direction),0,1);
	return ((ambient_color.rgb * ambient_color.a + direct_color));
}

vec3 specular(vec3 normal, vec3 halfway, float specular_power, float nl)
{
	float hightlight = pow(clamp(dot(normal,halfway),0,1), specular_power) * float(nl > 0.0);
	return light_color.xyz * specular_color.xyz * hightlight;
}

void main()
{
	vec4 albedo = texture(albedo_tex, texcoord_0);
	vec3 view = normalize(camera_world_pos - world_pos.xyz);
	vec3 l = normalize(light_direction);
	vec3 halfway = normalize(light_direction + l);
	vec3 n = normalize(normal);
	color.rgb =
	albedo.rgb * diffuse_color.rgb * diffuse(n, l)
	+ specular(normal, halfway, specular_power, dot(n, l));
	color.a = albedo.a * diffuse_color.a;
}
