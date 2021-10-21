
#version 330

in V2F{
	vec2 texcoord_0;
	vec3 normal;
	vec4 world_pos;
} v2f;

out vec4 color;

struct Per_instance
{
	// rgb = color, a = alpha
	vec4 diffuse_color;
	vec3 specular_color;
	vec3 ambient_color;
	// x = diffuse, y = specular intensity, z = specular power, 2 = ambient
	vec4 intensities;
	sampler2D albedo_tex;
};

struct Directional_light
{
	// rgb = color, a = intensity
	vec4 color;
	vec3 direction;
};


uniform Directional_light directional_light;
uniform Per_instance per_instance;
uniform vec3 camera_world_pos;


float diffuse(vec3 normal, vec3 light_direction)
{
	return clamp(dot(normal,light_direction),0,1);
}

float specular(vec3 normal, vec3 halfway, float specular_power, float nl)
{
	return pow(clamp(dot(normal,halfway),0,1), specular_power) * float(nl > 0.0);
}

void main()
{
	vec4 albedo = texture(per_instance.albedo_tex, v2f.texcoord_0);
	vec3 view = normalize(camera_world_pos - v2f.world_pos.xyz);
	vec3 l = normalize(directional_light.direction);
	vec3 halfway = normalize(view + l);
	vec3 n = normalize(v2f.normal);
	color.rgb = albedo.rgb * (
			per_instance.diffuse_color.rgb * diffuse(n, l) * per_instance.intensities.x +
			specular(n, halfway, per_instance.intensities.y, dot(n, l)) * per_instance.specular_color * per_instance.intensities.z +
			per_instance.ambient_color.rgb * per_instance.intensities.w
		);
	color.a = albedo.a * per_instance.diffuse_color.a;
}
